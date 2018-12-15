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
	Server(boost::asio::io_service& ios, uint16_t port);
	~Server();

private:
	boost::asio::io_service& ios;
	boost::asio::ip::tcp::acceptor acceptor;
};



#endif /*SERVER_HPP*/
