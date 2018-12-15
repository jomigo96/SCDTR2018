#include "Session.hpp"

Session::Session(boost::asio::io_service& ios):
	sock(ios){}

Session::~Session(){}

tcp::socket& Session::get_socket(){

	return sock;
}
