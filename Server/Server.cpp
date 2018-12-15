#include "Server.hpp"

Server::Server(boost::asio::io_service& ios, uint16_t port):
	ios(ios), acceptor(ios, tcp::endpoint(tcp::v4(), port)){

	std::shared_ptr<Session> session = std::make_shared<Session>(ios);
	acceptor.async_accept(session->get_socket(),
		[this, session](const boost::system::error_code& err){
			if(!err){
				session->start();
			}else{
				std::cerr << err.message() << std::endl;
				session->reset();
			}
		}
	);

}

Server::~Server(){}
