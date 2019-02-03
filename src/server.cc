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
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include "server.h"
#define BOOST_LOG_DYN_LINK 1
using boost::asio::ip::tcp;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;

// this function creates a new session object which in turn initializes a socket
// object. It then calls async_accept which allows server to accept
// data streams over its socket connection after a socket connection is established
bool server::start_accept()
{
    session* new_session = new session(io_service_); // create new session + initialize socket object
    acceptor_.async_accept(new_session->socket(), // wait for a socket connection in a thread
                           boost::bind(&server::handle_accept, this, new_session, // call handle_accept once a socket connection is established
                                       boost::asio::placeholders::error));
    waitingForConnection_ = true;
    return waitingForConnection_;
}

// handle_accept calls new_session->start() if a socket connection is established
bool server::handle_accept(session* new_session,
                           const boost::system::error_code& error)
{
    if (!error)
    {
        BOOST_LOG_TRIVIAL(info) << "TCP socket connection established";
        new_session->start(fileHandler, actionHandler);
        clientConnectionEstablished_ = true;
    }
    else
    {
        delete new_session;
        clientConnectionEstablished_ = false;
    }
    
    start_accept();
    return clientConnectionEstablished_;
}

// Sets up request handler objects
// TODO: Add echo/error handler here
void server::initRequestHandlers(NginxConfig* config)
{
    if(!config->staticPathMap.empty())
    {
        fileHandler = new StaticFileRequestHandler(config->staticPathMap, "/usr/src/projects/bigbear");
        actionHandler = new ActionRequestHandler(config->serverActionMap);
    }
    else
    {
        fileHandler = nullptr;
    }


}

void initLogging()
{
    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
    std::string format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%";
    logging::add_file_log(keywords::file_name="%N.log",
                          keywords::auto_flush = true,
                          keywords::rotation_size = 10 * 1024 * 1024,
                          keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
                          keywords::format = format);
    logging::add_console_log(std::clog, keywords::format = format);
    logging::add_common_attributes();
}
