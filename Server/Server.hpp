/*!
 * @file Server.hpp
 * @author João Gonçalves, Daniel Schiffart, Francisco Castro
 * @date 12 Jan 2019
 * 
 * @brief Server class header
 *
 * Server class interface, using boost asio library for async socket 
 * communications.
 *
 * */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
#include "Session.hpp"

using boost::asio::ip::tcp;

/*!
 * @class Server
 *
 * @brief Server class, multi-client async tcp acceptor.
 * */
class Server{

public:

    /*!
     * @brief Constructor
     * */
    Server(boost::asio::io_service& ios);

    /*!
     * @brief Destructor
     * */
    ~Server();

    /*!
     * @brief Server initialization
     *
     * @param port port number (usually 8000-8999)
     * */
    void start_sever(uint16_t port);

private:

    boost::asio::io_service& ios; //!< IO service

    boost::asio::ip::tcp::acceptor acceptor; 

    /*!
     * @brief client accept callback
     *
     * Launches a session upon connection
     * */
    void handle_accept(std::shared_ptr<Session> session,
                                         const boost::system::error_code& err);
};

#endif /*SERVER_HPP*/
