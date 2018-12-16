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
	std::stringstream out;

	for(auto it=args.cbegin(); it!=args.cend(); it++){
		if((*it).size() != 1){
			return std::string("Invalid command");
		}
	}

	if(args.size() == 3){

		switch(args[0][0]){
			case 's':
				return std::string("Not handled");
			case 'b':
				return std::string("Not handled");
			case 'g':
				switch(args[1][0]){
					case 'l':
						out << "l "<< args[2][0] << " <val>";
						break;
					case 'd':
						out << "d "<< args[2][0] << " <val>";
						break;
					case 's':
						out << "s "<< args[2][0] << " <val>";
						break;
					case 'L':
						out << "L "<< args[2][0] << " <val>";
						break;
					case 'o':
						out << "o "<< args[2][0] << " <val>";
						break;
					case 'r':
						out << "r "<< args[2][0] << " <val>";
						break;
					case 'p':
						out << "p "<< args[2][0] << " <val>";
						break;
					case 't':
						out << "t "<< args[2][0] << " <val>";
						break;
					case 'e':
						out << "e "<< args[2][0] << " <val>";
						break;
					case 'c':
						out << "c "<< args[2][0] << " <val>";
						break;
					case 'v':
						out << "v "<< args[2][0] << " <val>";
						break;
					default:
						out << "Unknown variable " << args[1][0];
						break;
				}
				break;
			default:
				return std::string("Invalid command");
		}
	}else{
		if(args[0][0] == 'r')
			return std::string("ack");
		else
			return std::string("Invalid command");
	}
	return out.str();
}
