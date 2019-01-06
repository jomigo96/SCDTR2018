/*!
 * @file Session.hpp
 * @author João Gonçalves, Daniel Schiffart, Francisco Castro
 * @date 12 Jan 2019
 * 
 * @brief Session class header
 *
 * Client session handler.
 *
 * */

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

/*!
 * @class Session
 *
 * Inherits from enable_shared_from_this so as to be able to create shared 
 * pointers to itself and therefore control its own lifetime.
 * */
class Session : public std::enable_shared_from_this<Session>{

public:

    /*!
     * @brief Constructor
     * */
    Session(boost::asio::io_service& ios);

    /*!
     * @brief Destructor
     * */
    ~Session();

    /*!
     * @brief tcp socket getter
     * */
    tcp::socket& get_socket();

    /*!
     * @brief session start, listens on socket for a command until a '\n'
     * */
    void start();

private:
    boost::asio::io_service& ios; //!< IO service
    tcp::socket sock; //!< tcp socket
    boost::asio::io_service::strand write_strand; //!< Strand for robustness
    boost::asio::streambuf in_buf; //!< Input buffer
    std::string out_data; //!< Reply buffer (as a string)

    char stream_var; //!< Variable currently streamming (l or d)
    int stream_node; //!< Node whose data is being streammed (0 or 1)
    bool stream_stop; //!< Flag that signals that streamming should stop

    /*!
     * @brief Callback for socket reads
     *
     * Parses client command and either fetches the required data or starts
     * streamming mode.
     * */
    void handle_read(std::shared_ptr<Session>& s, 
                          const boost::system::error_code& err, size_t byte_n);

    /*!
     * @brief Callback for the write operation
     *
     * Starts to listen again for a new command.
     * */
    void handle_write(std::shared_ptr<Session>& s,
                               const boost::system::error_code& err, size_t n);

    /*!
     * @brief Gather requested data to be sent.
     *
     * Return value is a string, to be stored in the output buffer. C++11 
     * should optimize the copy assignment.
     *
     * @param args command arguments, should already be checked for size.
     * */
    std::string fetch_data(const std::vector<std::string>& args) const;

    /*!
     * @brief Send the output buffer contents and start listening for a new
     * command
     * */
    void send_reply(void);

    /*!
     * @brief Start streamming mode
     *
     * Session passively waits for a new sample to be available then sends it.
     * */
    void start_streaming(std::shared_ptr<Session>& s, const boost::system::error_code& err, size_t n);

    /*!
     * @brief Sends the output buffer contents, and return to streamming mode
     * */
    void send_sample();
};




#endif /*SESSION_HPP*/
