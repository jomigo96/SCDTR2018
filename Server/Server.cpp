#include "Server.hpp"

Server::Server(boost::asio::io_service& ios):
    ios(ios), acceptor(ios){
}

Server::~Server(){}

void Server::start_sever(uint16_t port){
    std::shared_ptr<Session> session = std::make_shared<Session>(ios);
    tcp::endpoint endpoint(tcp::v4(), port);
    acceptor.open(endpoint.protocol());
    acceptor.set_option(tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen();

    acceptor.async_accept(session->get_socket(),
                        [=](auto ec){
                            this->handle_accept(session, ec);
                        });
}

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
