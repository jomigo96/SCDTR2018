#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <mutex>
#include <iostream>
#include <string>
#include <vector>
#include <condition_variable>
#include "data_structures.hpp"

#define DATA_SIZE 1024

using boost::asio::ip::tcp;

extern std::mutex m;
extern std::condition_variable cv;
extern bool ready;

class Session : public std::enable_shared_from_this<Session>{

public:
	Session(boost::asio::io_service& ios);
	~Session();

	tcp::socket& get_socket();

	void start();



private:
	tcp::socket sock;
	char data[DATA_SIZE];

	void handle_read(std::shared_ptr<Session>& s, const boost::system::error_code& err, size_t byte_n);
	void handle_write(std::shared_ptr<Session>& s, const boost::system::error_code& err);
	std::string fetch_data(const std::vector<std::string>& args) const;
};




#endif /*SESSION_HPP*/
