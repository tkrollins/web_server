#include "gtest/gtest.h"
#include "server.h"
#include <boost/asio.hpp>

class SessionTest : public ::testing::Test 
{
  	protected:
    		boost::asio::io_service io_service_;
    		session new_session = session(io_service_);
};

// The method, renderResponse, should return a properly-formatted HTTP 200 response to the client
// upon receiving a valid HTTP request
TEST_F(SessionTest, RenderResponse200) 
{
  	// take an example HTTP request and compare the expected response with renderResponse's return
  	std::string s = "GET /index.html HTTP/1.1\r\nUser-Agent: nc/0.0.1\r\nHost: 127.0.0.1\r\nAccept: */*\r\n\r\n";

    // call renderResponse
  	std::string outStr;
  	outStr = new_session.renderResponse(s);

  	std::string res = "";
  	std::string h = "HTTP/1.1 200 OK\r\n";                                                                              
  	std::string type = "Content-Type: text/plain\r\n"; // did't put \r\n here, since the parse_http_request offers
  	std::string content = std::string(s);
  	std::string contentLength = "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n";
  	// input with leading \r\n
  	res = h + type + contentLength + content;
  	
	// used STREQ instead of EQ because we want to compare the strings, not the memory locations of 
	// pointers
  	EXPECT_STREQ(res.c_str(), outStr.c_str()); 
}
/*
// The method, render_response, should write an error message back to client upon receiving a bad
// HTTP request
TEST_F(SessionTest, RenderResponse400Error)
{
    // create the malformed HTTP request
    std::string badRequest = "1234asdf";

    // This is the error message that the server should return
    std::string errorMessage = "HTTP Error 400 - Bad Request\r\n\r\n";

    // call renderResponse
    std::string outStr;
    outStr = new_session.renderResponse(badRequest);

    EXPECT_STREQ(errorMessage.c_str(), outStr.c_str()); 
}
 */