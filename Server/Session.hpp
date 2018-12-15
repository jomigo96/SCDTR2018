#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
#include "Session.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>{

public:
	Session(boost::asio::io_service& ios);
	~Session();

	tcp::socket& get_socket();

	void start(){}

	void reset(){}

private:
	tcp::socket sock;

};




#endif /*SESSION_HPP*/
