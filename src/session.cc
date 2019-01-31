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
#include "http_response.h"
#include "http_request.h"
#include <string>

using boost::asio::ip::tcp;

tcp::socket& session::socket()
{
    return socket_;
}

void session::start()
{
    printf("Accepting data over socket:\n");
    
    socket_.async_read_some(boost::asio::buffer(data_, max_length), // read incoming data in a new thread (data_ contains this information)
                            boost::bind(&session::handleRead, this, // once done, call session::handleRead
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void session::handleRead(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (!error)
    {
        assert (data_!=NULL);
        // cast the data read from the socket to a pointer to the first char in the char sequence
        char* socketReadBuffer  = static_cast<char*> (data_);

        // print statements for debugging purposes
        printf("Data: %s\n", socketReadBuffer);

        std::string tmp = "";
        session::parseRequest(std::string(socketReadBuffer));
      
        printf("Socket data read. Writing response data to socket...\n");

        std::string res;
        char reqBody[1024];

        res = session::renderResponse(std::string(socketReadBuffer));

        boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
                                 boost::asio::buffer(res, res.length()), // the read data that will be written to socket_
                                 boost::bind(&session::handleWrite, this, // call session::handleWrite() once done writing
                                             boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
}

bool session::parseRequest(std::string socketReadBuffer)
{
    HttpRequest requestMaker; // TODO: rewrite below function to httprequest

    // The following loop detects the end of an HTTP message by using a naive blocking implementation: 
    // iterates through the received message until you find \n\n or \r\n\r\n. The loop stores the pointer 
    // to the end of the HTTP message into requestEndPtr
    // TODO: parse rest of the buffer in the for loop to determine if it's an HTTP request and of what type. 
    // Can also refactor this loop into a member function of the class containing data_. Additional 
    // attributes should be added to the class accordingly, such as requestEndPtr and perhaps 
    // HTTP request type 
    char* requestEndPtr;
    char* iterativeBuffer = new char[socketReadBuffer.length() + 1];
    strcpy(iterativeBuffer, socketReadBuffer.c_str());
    for (char* it = iterativeBuffer; *it; it++)
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
}

std::string session::renderResponse(std::string inputStr)
{
    std::string res;
    HttpResponse responseMaker;
    std::string status = "200";
    // Using unordered map for header is acceptable, since accouding to RFC 2616 the order of headerr fields doesn't matter
    // However, it is good practice to send general-header first
    // So someday when we're going above and beyond we fix it then
    
    std::unordered_map<std::string,std::string> headers;
    headers["Content-Type"] = "text/plain";
    headers["Content-Length"] = std::to_string(inputStr.length());
    
    res = responseMaker.buildHttpResponse(status, headers, inputStr);
    return res;
}

void session::handleWrite(const boost::system::error_code& error)
{
    if (!error)
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length), // read more data out of the buffer
                          boost::bind(&session::handleRead, this, // call handleRead again once you are done reading
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
        socket_.close();
    }
    else
    {
        delete this;
    }
}
