#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Server.hpp"

int main(){

	try{
		boost::asio::io_service io;
		Server ser(io, 8080);
		io.run();

	}catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
