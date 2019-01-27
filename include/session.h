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

using boost::asio::ip::tcp;
using ::testing::_;
using ::testing::Invoke;

class session
{
	public:
		session(boost::asio::io_service& io_service) : socket_(io_service) {};
		tcp::socket& socket();

		void start();
		virtual std::string render_response(std::string inputStr);
	private:
		void handle_read(const boost::system::error_code& error,
			size_t bytes_transferred);

		void handle_write(const boost::system::error_code& error);

		tcp::socket socket_;
		enum { max_length = 1024 };
		char data_[max_length];
		bool parse_http_request(char* inputStr, char* requestBody);
};

class MockSession : public session
{
	public:
		MockSession(boost::asio::io_service& io_service) : session(io_service), real_(trueIOS)
		{
			ON_CALL(*this, render_response(_))
				.WillByDefault(Invoke(&real_, &session::render_response));
		}
		MOCK_METHOD1(render_response, std::string(std::string inputStr));
	private:
		boost::asio::io_service trueIOS;
		session real_;
};

