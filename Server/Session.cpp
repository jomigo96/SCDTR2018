#include "Session.hpp"

Session::Session(boost::asio::io_service& ios):
	ios(ios), sock(ios), write_strand(ios){}

Session::~Session(){}

tcp::socket& Session::get_socket(){

	return sock;
}

void Session::handle_write(std::shared_ptr<Session>& s, const boost::system::error_code& err, size_t n){
	if(!err){
		std::cout << "Sent " << n << " bytes" << std::endl;
	    boost::asio::async_read_until(sock,
	        						in_buf,
									'\n',
	        						boost::bind(&Session::handle_read, this,
	                    						shared_from_this(),
	                    						boost::asio::placeholders::error,
	                    						boost::asio::placeholders::bytes_transferred));
	    }else{
	    	std::cerr << "Error: " << err.message() << std::endl;
			s.reset();
	}
}

void Session::handle_read(std::shared_ptr<Session>& s, const boost::system::error_code& err, size_t byte_n){

	if(!err){

		std::cout << "Received " << byte_n << " bytes" << std::endl;

		// Parse code
		std::istream message(&in_buf);
		std::vector<std::string> args;
		std::string buff;

		while(message >> buff)
			args.push_back(buff);

		// check argument count
		if((args.size() != 1) && (args.size() != 3)){
			out_data = std::string("Invalid command number");
		}else{
			out_data = this->fetch_data(args);
		}
		std::cout << "Sending back: " << out_data << std::endl;
		out_data += '\n';

		ios.post(write_strand.wrap([me=shared_from_this()]
								(){
									me->send_reply();
								}));


	}else{
		std::cerr << "Error: " << err.message() << std::endl;
		s.reset();
	}
}

void Session::send_reply(){

	boost::asio::async_write(sock,
							boost::asio::buffer(out_data),
							write_strand.wrap(boost::bind(&Session::handle_write,
														  this,
														  shared_from_this(),
													  	  boost::asio::placeholders::error,
														  boost::asio::placeholders::bytes_transferred
													  )));

}

void Session::start(){
	boost::asio::async_read_until(sock,
								  in_buf,
								  '\n',
								  boost::bind(&Session::handle_read,
											  this,
											  shared_from_this(),
											  boost::asio::placeholders::error,
											  boost::asio::placeholders::bytes_transferred));
}


std::string Session::fetch_data(const std::vector<std::string>& args) const{
	// I know this swich is ugly af
	std::stringstream out;

	for(auto it=args.cbegin(); it!=args.cend(); it++){
		if((*it).size() != 1){
			out << "Invalid command";
			return out.str();
		}
	}

	if(args.size() == 3){

		if((args[2][0] != '1')&&(args[2][0] != '2')&&(args[2][0] != 'T')){
			out << "Invalid command";
			return out.str();
		}

		int desk;
		if(args[2][0] == 'T')
			desk = -1;
		else
			desk = args[2][0] - '1';

		m.lock();

		switch(args[0][0]){
			case 's':
				out << "Not handled";
				break;
			case 'b':
				{
					char c = args[1][0];
					if((desk != -1)&&((c == 'l')||(c == 'd'))){
						auto &desk_values = (desk == 0) ? last_minute_buffer.first : last_minute_buffer.second;

						out << "b " << args[1][0] << " " << desk+1;

						for(auto it = desk_values.cbegin(); it!=desk_values.cend(); it++){
							out << ", " << ((c == 'l') ? (*it).first : (*it).second);
						}
					}else{
						out << "Invalid";
					}
				}
				break;
			case 'g':
				switch(args[1][0]){
					case 'l':
						if(desk != -1){
							out << "l "<< desk+1 << " " << desks[desk].illuminance;
						}else{
							out << "Invalid command";
						}
						break;
					case 'd':
						if(desk != -1){
							out << "d " << desk+1 << " " << desks[desk].duty_cycle;
						}else{
							out << "Invalid command";
						}
						break;
					case 's':
						if(desk != -1){
							out << "s "<< desk+1 << " " << desks[desk].occupancy;
						}else{
							out << "Invalid command";
						}
						break;
					case 'L':
						if(desk != -1){
							out << "L "<< desk+1 << " " << desks[desk].illuminance_lb;
						}else{
							out << "Invalid command";
						}
						break;
					case 'o':
						if(desk != -1){
							out << "o "<< desk+1 << " " << desks[desk].illuminance_bg;
						}else{
							out << "Invalid command";
						}
						break;
					case 'r':
						if(desk != -1){
							out << "r "<< desk+1 << " " << desks[desk].illuminance_ref;
						}else{
							out << "Invalid command";
						}
						break;
					case 'p':
						if(desk != -1){
							out << "p "<< desk+1 << " " << desks[desk].power;
						}else{
							out << "p T " << desks[0].power + desks[1].power;
						}
						break;
					case 't':
						if(desk != -1){
							uint32_t time_now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
							out << "t "<< desk+1 << " " << (float)(time_now-desks[desk].time_acc)/1000.0;
						}else{
							out << "Invalid command";
						}
						break;
					case 'e':
						if(desk != -1){
							out << "e "<< desk+1 << " " << desks[desk].energy_acc;
						}else{
							out << "e T " << desks[0].energy_acc + desks[1].energy_acc;
						}
						break;
					case 'c':{
						double error;
						if(desk != -1){
							if(desks[desk].comf_err_samples > 0){
								error = desks[desk].comfort_error_acc/(double)desks[desk].comf_err_samples;
							}else{
								error = 0;
							}
							out << "c "<< desk+1 << " " << error;
						}else{
							if (desks[0].comf_err_samples > 0) {
								error = desks[0].comfort_error_acc/(double)desks[0].comf_err_samples;
							} else {
								error = 0;
							}
							if (desks[1].comf_err_samples > 0) {
								error += desks[1].comfort_error_acc/(double)desks[1].comf_err_samples;
							}
							out << "c T " << error;
						}
						break;}
					case 'v':{
						double error;
						if(desk != -1){
							if(desks[desk].samples>0){
								error = desks[desk].comfort_flicker_acc/(double)desks[desk].samples;
							}else{
								error = 0;
							}
							out << "v "<< desk+1 << " " << error;
						}else{
							if(desks[0].samples>0){
								error = desks[0].comfort_flicker_acc/(double)desks[0].samples;
							}else{
								error = 0;
							}
							if(desks[1].samples>0){
								error += desks[1].comfort_flicker_acc/(double)desks[1].samples;
							}
							out << "c T " << error;
						}
						break;}
					default:
						out << "Unknown variable " << args[1][0];
						break;
				}
				break;
			default:
				out << "Invalid command";
		}

		m.unlock();

	}else{
		if(args[0][0] == 'r'){
			uint32_t time_now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			m.lock();
			memset(desks, 0, 2*sizeof(desk_t));
			desks[0].time_acc = time_now;
			desks[1].time_acc = time_now;
			last_minute_buffer.first.clear();
			last_minute_buffer.second.clear();
			m.unlock();
			out << "ack";
		}else{
			out << "Invalid command";
		}
	}
	return out.str();
}
