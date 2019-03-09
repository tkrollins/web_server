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
#include "http_request.h"
#include <string>
#include <boost/log/trivial.hpp>
#include <map>

using boost::asio::ip::tcp;

tcp::socket& session::socket()
{
    return socket_;
}

void session::start(HandlerManager* manager, NginxConfig* config)
{
    sessionDispatcher = std::make_unique<RequestHandlerDispatcher>(RequestHandlerDispatcher(*config));
    sessionManager = manager;
    sessionConfig = config;
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&session::handleRead, this, // once done reading, call session::handleRead
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void session::writeToSocket(std::string response)
{
    boost::asio::async_write(socket_, // socket_ is the destination in which read data is to be written to
                             boost::asio::buffer(response), // the read data that will be written to socket_
                             boost::bind(&session::handleWrite, this, // call session::handleWrite() once done writing
                                         boost::asio::placeholders::error,
                                         response,
                                         boost::asio::placeholders::bytes_transferred));
}

void session::handleRead(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (!error)
    {
        assert (data_ != nullptr);

        boost::system::error_code ec;
        boost::asio::ip::tcp::endpoint endpoint = socket_.remote_endpoint(ec);
        if (!ec) { BOOST_LOG_TRIVIAL(info) << "Client IP: " << endpoint.address().to_string(); }

        std::string socketReadBuffer  = std::string(data_);
        HttpRequest receivedRequest;
        bool isParseCompleted = receivedRequest.parseHttpRequest(socketReadBuffer);
        std::cout << "MAL: " << receivedRequest.isMalformed << std::endl;
        if(isParseCompleted)
        {
            std::unique_ptr<HttpResponse> response = sessionDispatcher->dispatchHandler(receivedRequest, sessionManager, *sessionConfig);
            BOOST_LOG_TRIVIAL(info) << "::Response Code::" << response -> errorCode;
            writeToSocket(response -> buildHttpResponse());
        }
        else
        {
            printf("incomplete request\n");
            // wait for rest of http request to come in, and then finish reading HTTP request
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&session::finishRead, this,
                                                boost::asio::placeholders::error,
                                                receivedRequest));
        }
    }
    else
    {
        delete this;
    }
}

void session::handleWrite(const boost::system::error_code& error, std::string response, size_t bytes_transferred)
{
    if(!error && (bytes_transferred < response.size()))
    {
        BOOST_LOG_TRIVIAL(debug) << "Bytes written:  " << bytes_transferred << " / " << response.size();
        response = response.substr(bytes_transferred, std::string::npos);
        writeToSocket(response);
    }
    else if (!error)
    {
        BOOST_LOG_TRIVIAL(debug) << "Bytes written:  " << bytes_transferred << " / " << response.size();
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

void session::finishRead(const boost::system::error_code& error, HttpRequest request)
{
    if (!error)
    {    
        std::string socketReadBuffer  = std::string(data_);
        bool isParseCompleted = request.finishParsingRequest(socketReadBuffer);

        if(isParseCompleted)
        {
            std::unique_ptr<HttpResponse> response = sessionDispatcher->dispatchHandler(request, sessionManager, *sessionConfig);
            BOOST_LOG_TRIVIAL(info) << "::Response Code::" << response -> errorCode;
            writeToSocket(response -> buildHttpResponse());
        }
        else
        {
            printf("incomplete request\n");
            // wait for rest of http request to come in, and then finish reading HTTP request
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&session::finishRead, this,
                                                boost::asio::placeholders::error,
                                                request));
        }
    }
    else
    {
        delete this;
    }
}