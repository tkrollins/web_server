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
#include <string>

using boost::asio::ip::tcp;

tcp::socket& session::socket()
{
  return socket_;
}

void session::start()
{
  printf("TCP Socket Established. Accepting data over socket:\n");

  socket_.async_read_some(boost::asio::buffer(data_, max_length), // read incoming data in a new thread (data_ contains this information)
      boost::bind(&session::handle_read, this, // once done, call session::handle_read
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
  if (!error)
  {
    assert (data_!=NULL);

    // cast the data read from the socket to a pointer to the first char in the char sequence
    char* socketReadBuffer  = static_cast<char*> (data_);

    // print statements for debugging purposes
    printf("Data: ");
    printf("%s", socketReadBuffer);
    printf("\n");

    // The following loop detects the end of an HTTP message by using a naive blocking implementation: 
    // iterates through the received message until you find \n\n or \r\n\r\n. The loop stores the pointer 
    // to the end of the HTTP message into requestEndPtr
    // TODO: parse rest of the buffer in the for loop to determine if it's an HTTP request and of what type. 
    // Can also refactor this loop into a member function of the class containing data_. Additional 
    // attributes should be added to the class accordingly, such as requestEndPtr and perhaps 
    // HTTP request type 
    char* requestEndPtr;
    for (char* it = socketReadBuffer; *it; it++)
    {
        if (*it == '\n' && (*it+1) == '\n') // Linux HTTP request
        {
          printf("end of HTTP request detected \n");
          requestEndPtr = it;
          break;
        }
        else if (*it == '\r' && // Windows HTTP request
                (*(it+1)) == '\n' && 
                (*(it+2)) == '\r' && 
                (*(it+3)) == '\n')
        {
          printf("end of HTTP request detected \n");
          requestEndPtr = it;
          break;
        }
    }

    // TODO: instead of writing the entire buffer -- boost::asio::buffer(data_, bytes_transferred) -- to
    // the socket, send an HTTP 200 response + the request in the buffer if the buffer indeed contains a
    // request. This is based on how I understand the assignment directions, but I might be wrong.
    
    printf("Socket data read. Writing response data to socket...\n");


    char res[1024];
    char reqBody[1024];
    bool result = session::parse_http_request(data_, reqBody);
    if (!result){ // parsing http result error
      return;
    }

    session::render_response(reqBody, res);

    boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
        boost::asio::buffer(res, strlen(res)), // the read data that will be written to socket_
        boost::bind(&session::handle_write, this, // call session::handle_write() once done writing
          boost::asio::placeholders::error));
  }
  else
  {
    delete this;
  }
}

bool session::parse_http_request(char* inputStr, char* requestBody) // TODO: Maybe we would need other part of the request?
{
  std::string str(inputStr);
  std::string str2 = str.substr(str.find("\r\n\r\n"));
  strcpy(requestBody, str2.c_str());

  return true;
}

void session::render_response(char* inputStr, char* outStr)
{
  std::string res = "";
  std::string h = "HTTP/1.1 200 OK\r\n";                                                                              
  std::string type = "Content-Type: text/plain"; // did't put \r\n here, since the parse_http_request offers
  // input with leading \r\n
  std::string content = inputStr;
  res = h + type + content + "\r\n";

  strcpy(outStr, res.c_str());
}

void session::handle_write(const boost::system::error_code& error)
{
  if (!error)
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length), // read more data out of the buffer
        boost::bind(&session::handle_read, this, // call handle_read again once you are done reading
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    socket_.close();
  }
  else
  {
    delete this;
  }
}