#include "gtest/gtest.h"
#include "server.h"
#include <boost/asio.hpp>

class ServerTest : public ::testing::Test 
{
  	protected:
    		boost::asio::io_service io_service;
    		short listenPort = 8080;
    		server testServer = server(io_service, listenPort);
};

// the server should already be waiting for a TCP connection because the constructor
// calls the start_accept method, which makes the server start accepting new TCP connections
TEST_F(ServerTest, startedAccept)
{
	EXPECT_TRUE(testServer.getConnectionWaitingStatus());
}

// The server should not be connected to any client upon initialization of the server object
TEST_F(ServerTest, initiallyNoSocket)
{
    // create the malformed HTTP request
    std::string badRequest = "1234asdf";

    // This is the error message that the server should return
    std::string errorMessage = "HTTP Error 400 - Bad Request\r\n\r\n";

    // call render_response
	session new_session(io_service);
    std::string outStr;
    outStr = new_session.render_response(badRequest);

    EXPECT_STREQ(errorMessage.c_str(), outStr.c_str()); 
}
