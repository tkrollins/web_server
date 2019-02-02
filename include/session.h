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
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "static_file_request_handler.h"

using boost::asio::ip::tcp;
using ::testing::_;
using ::testing::Invoke;

class session
{
public:
    session(boost::asio::io_service& io_service) : socket_(io_service) {};
    tcp::socket& socket();

    void start(StaticFileRequestHandler* handler);
    virtual std::string renderResponse(std::string inputStr);
private:
    StaticFileRequestHandler* sessionFileHandler;
    void handleRead(const boost::system::error_code& error,
                    size_t bytes_transferred);

    void handleWrite(const boost::system::error_code& error);
    bool parseRequest(std::string inputStr);
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    bool parse_http_request(char* inputStr, char* requestBody);
    bool isValidRequest(std::string inputStr);
};

class MockSession : public session
{
public:
    MockSession(boost::asio::io_service& io_service) : session(io_service), real_(trueIOS)
    {
        ON_CALL(*this, renderResponse(_))
                .WillByDefault(Invoke(&real_, &session::renderResponse));
    }
    MOCK_METHOD1(renderResponse, std::string(std::string inputStr));
private:
    boost::asio::io_service trueIOS;
    session real_;
};

