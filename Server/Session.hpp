#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <mutex>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <deque>
#include <condition_variable>
#include "data_structures.hpp"

#define DATA_SIZE 1024

using boost::asio::ip::tcp;

extern std::mutex m;
extern std::condition_variable cv;
extern bool ready;
extern desk_t desks[];
extern std::pair< std::deque<std::pair<float, float>> , std::deque<std::pair<float, float> > > last_minute_buffer;

class Session : public std::enable_shared_from_this<Session>{

public:
    Session(boost::asio::io_service& ios);
    ~Session();

    tcp::socket& get_socket();

    void start();



private:
    boost::asio::io_service& ios;
    tcp::socket sock;
    boost::asio::io_service::strand write_strand;
    boost::asio::streambuf in_buf;
    std::string out_data;

    char stream_var;
    int stream_node;
    bool stream_stop;

    void handle_read(std::shared_ptr<Session>& s, const boost::system::error_code& err, size_t byte_n);
    void handle_write(std::shared_ptr<Session>& s, const boost::system::error_code& err, size_t n);
    std::string fetch_data(const std::vector<std::string>& args) const;
    void send_reply(void);
    void start_streaming(std::shared_ptr<Session>& s, const boost::system::error_code& err, size_t n);
    void send_sample();
};




#endif /*SESSION_HPP*/
