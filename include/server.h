//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <vector>
#include "session.h"
#include "static_file_request_handler.h"
#include "action_request_handler.h"
#include "error_request_handler.h"
#include "config_parser_old.h"
#include "config_parser.h"
using boost::asio::ip::tcp;


class server
{
public:
    //TODO: refactor server construtor to use NginxConfig object, and init HandlerManager/Dispatcher

    server(boost::asio::io_service& io_service, short port, NginxConfig_old* config)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) // establishes server endpoint
        {
            // this function creates a new session object which in turn initializes a socket
            // object. It then calls async_accept which allows server to accept
            // data streams over its socket connection after a socket connection is established
            //initLogging();
            initRequestHandlers(config);
            start_accept();
        }

    bool getConnectionWaitingStatus()
    {
        return waitingForConnection_;
    }

    bool getConnectionStatus()
    {
        return clientConnectionEstablished_;
    }

    StaticFileRequestHandler* fileHandler;
    ActionRequestHandler* actionHandler;

private:
    bool start_accept();

    //TODO: remove this function, call dispatcher instead
    void initRequestHandlers(NginxConfig_old* config);
    //TODO: remove
    std::vector<RequestHandler*> requestHandlers;


    bool handle_accept(session* new_session,
        const boost::system::error_code& error);
  
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;

    // boolean attributes for the state of the server's TCP connection(s)
    bool waitingForConnection_ = false;
    bool clientConnectionEstablished_ = false;
};

void initLogging();
