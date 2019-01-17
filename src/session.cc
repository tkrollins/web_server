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

#include "session.h"

using boost::asio::ip::tcp;

tcp::socket& session::socket()
{
  return socket_;
}

void session::start()
{
  printf("hello2\n"); // debugging purposes
  // TODO: In this function, detect when the end of the HTTP request is in order to reach the read
  // callback function. Currently, handle_read isn't being called
  socket_.async_read_some(boost::asio::buffer(data_, max_length), // read incoming data in a new thread
      boost::bind(&session::handle_read, this, // once done, call session::handle_read
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
  if (!error)
  {
    printf("hello\n"); // debugging purposes
    boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
        boost::asio::buffer(data_, bytes_transferred), // the read data
        boost::bind(&session::handle_write, this, // call session::handle_write() once done writing
          boost::asio::placeholders::error));
  }
  else
  {
    delete this;
  }
}

void session::handle_write(const boost::system::error_code& error)
{
  if (!error)
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length), // read more data out of the buffer
        boost::bind(&session::handle_read, this, // call handle_read again once you are done reading
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }
  else
  {
    delete this;
  }
}