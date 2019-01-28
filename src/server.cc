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

#include "server.h"

using boost::asio::ip::tcp;

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

  printf("Accepting TCP socket connections\n");

  return waitingForConnection_;
}

// handle_accept calls new_session->start() if a socket connection is established
bool server::handle_accept(session* new_session,
    const boost::system::error_code& error)
{
  if (!error)
  {
    printf("TCP socket connection established\n");
    new_session->start();

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
