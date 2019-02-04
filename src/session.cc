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

void session::start(StaticFileRequestHandler* staticFileHandler, ActionRequestHandler* actionRequestHandler)
{
    sessionFileHandler = staticFileHandler;
    sessionActionReqHandler = actionRequestHandler; 
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
        bool successfullyParsedReq = receivedRequest.parseHttpRequest(socketReadBuffer);

        boost::system::error_code ec;
        boost::asio::ip::tcp::endpoint endpoint = socket_.remote_endpoint(ec);
        if (!ec)
            BOOST_LOG_TRIVIAL(info) << "Client IP: " << endpoint.address().to_string();
        
        // This is where the request handler is called
        //
        bool isStaticReq = false;
        bool isActionReq = false;
        if(sessionFileHandler != nullptr && sessionActionReqHandler != nullptr)
        {
            isStaticReq = sessionFileHandler->canHandleRequest(receivedRequest);
            isActionReq = sessionActionReqHandler->canHandleRequest(receivedRequest);
        }
        if(isStaticReq && successfullyParsedReq)
        {
            // These write calls are pretty wonky but it was the only way I could get if functioning
            // TODO: refactor this code
            std::string headerToSend;
            std::vector<char> fileToSend;
            sessionFileHandler->handleRequest(&headerToSend, &fileToSend);
            if(!socket_.is_open()) { return; }// will not open during testing
            boost::asio::write(socket_, boost::asio::buffer(headerToSend));
            boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
                                     boost::asio::buffer(fileToSend), // the read data that will be written to socket_
                                     boost::bind(&session::handleWrite, this, // call session::handleWrite() once done writing
                                                 boost::asio::placeholders::error));
        }
        // This is left in to allow echo handling operate
        // if the request is for a server action, handle request through the session's action request handler
        else if(isActionReq && successfullyParsedReq)
        {
            printf("handling action request\n");
            response = sessionActionReqHandler->handleRequest(receivedRequest);
            boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
                                     boost::asio::buffer(response, response.length()), // the read data that will be written to socket_
                                     boost::bind(&session::handleWrite, this, // call session::handleWrite() once done writing
                                                 boost::asio::placeholders::error));
        }
        // the the request is not asking for a static file or for an action, reply with bad request message
        else
        {
            std::string badRequestStatus = "400";
            std::string body = "400 Error: Bad request\n";
            std::string contentLengthStr = std::to_string(body.length());
            std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                        {"Content-Length", contentLengthStr}};
            
            HttpResponse httpResponse;
            response = httpResponse.buildHttpResponse(badRequestStatus, headers, body);
            boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
                                     boost::asio::buffer(response, response.length()), // the read data that will be written to socket_
                                     boost::bind(&session::handleWrite, this, // call session::handleWrite() once done writing
                                                 boost::asio::placeholders::error));
        }
    }
    else
    {
        delete this;
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
