#include "Server.hpp"

Server::Server(boost::asio::io_service& ios, uint16_t port):
	ios(ios), acceptor(ios, tcp::endpoint(tcp::v4(), port)){

	std::shared_ptr<Session> session = std::make_shared<Session>(ios);
	acceptor.async_accept(session->get_socket(),
						boost::bind(&Server::handle_accept,
									this,
									session,
									boost::asio::placeholders::error));

}

Server::~Server(){}

void Server::handle_accept(std::shared_ptr<Session> session, const boost::system::error_code& err){

	if(!err){
		session->start();
		session = std::make_shared<Session>(ios);
		acceptor.async_accept(session->get_socket(),
							boost::bind(&Server::handle_accept,
										this,
										session,
										boost::asio::placeholders::error));

	}else{
		std::cerr << "error: " << err.message() << std::endl;
		session.reset();
	}

}
