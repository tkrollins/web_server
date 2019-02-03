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
#include <boost/log/trivial.hpp>
#include <map>

using boost::asio::ip::tcp;

tcp::socket& session::socket()
{
    return socket_;
}

void session::start(StaticFileRequestHandler* handler)
{
    sessionFileHandler = handler;
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
        
        std::string socketReadBuffer  = std::string(data_);
        HttpRequest receivedRequest;
        receivedRequest.parseHttpRequest(socketReadBuffer);


        // session::parseRequest(std::string(socketReadBuffer));
        BOOST_LOG_TRIVIAL(info) << "Client IP: " << socket_.remote_endpoint().address().to_string();

        // This is where the request handler is called
        //
        bool isStaticReq = false;
        if(sessionFileHandler != nullptr)
        {
            isStaticReq = sessionFileHandler->canHandleRequest(receivedRequest);
        }
        if(isStaticReq)
        {
            // These write calls are pretty wonky but it was the only way I could get if functioning
            // TODO: refactor this code
            std::string headerToSend;
            std::vector<char> fileToSend;
            sessionFileHandler->handleRequest(&headerToSend, &fileToSend);
            boost::asio::write(socket_, boost::asio::buffer(headerToSend));
            boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
                                     boost::asio::buffer(fileToSend), // the read data that will be written to socket_
                                     boost::bind(&session::handleWrite, this, // call session::handleWrite() once done writing
                                                 boost::asio::placeholders::error));
        }
        // This is left in to allow echo handling operate
        // TODO: Add echo and error handlers here. /echo will be for echo, everything else will be error
        else
        {
            std::string res = session::isValidRequest(socketReadBuffer)? session::renderResponse(socketReadBuffer) : "Bad Request!";
            boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
                                     boost::asio::buffer(res, res.length()), // the read data that will be written to socket_
                                     boost::bind(&session::handleWrite, this, // call session::handleWrite() once done writing
                                                 boost::asio::placeholders::error));
        }
        }
    else
    {
        delete this;
    }
}

// define the valid http request to have "GET" as first word
bool session::isValidRequest(std::string inputStr)
{
    if(inputStr.length() < 3)
        return false;
    return inputStr.substr(0, 3).compare("GET") == 0;
}

std::string session::renderResponse(std::string inputStr)
{
    std::string res;
    HttpResponse responseMaker;
    std::string status = "200";
    // Using unordered map for header is acceptable, since accouding to RFC 2616 the order of headerr fields doesn't matter
    // However, it is good practice to send general-header first
    // So someday when we're going above and beyond we fix it then
    std::map<std::string,std::string> headers;
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
