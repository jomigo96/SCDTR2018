#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
#include "Session.hpp"

using boost::asio::ip::tcp;

class Server{
public:
    Server(boost::asio::io_service& ios);
    ~Server();
    void start_sever(uint16_t port);

private:
    boost::asio::io_service& ios;
    boost::asio::ip::tcp::acceptor acceptor;

    void handle_accept(std::shared_ptr<Session> session, const boost::system::error_code& err);
};



#endif /*SERVER_HPP*/
