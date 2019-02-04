#include "gtest/gtest.h"
#include "server.h"
#include "static_file_request_handler.h"
#include "action_request_handler.h"
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>

using ::testing::Return;
using ::testing::_;

class SessionTest : public ::testing::Test
{
    protected:
            boost::asio::io_service io_service_;
            session new_session = session(io_service_);
            boost::system::error_code err;
            size_t bytesTransferred;
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

    std::string content = s;
    std::string res = "";
    std::string h = "HTTP/1.1 200 OK\r\n";
    std::string contentLength = "Content-Length: " + std::to_string(content.length()) + "\r\n";
    std::string type = "Content-Type: text/plain\r\n\r\n"; // did't put \r\n here, since the parse_http_request offers

    // input with leading \r\n
    res = h + contentLength + type + content;

    std::cout << res << std::endl;
    std::cout << outStr << std::endl;

    
    // used STREQ instead of EQ because we want to compare the strings, not the memory locations of 
    // pointers
    EXPECT_STREQ(res.c_str(), outStr.c_str()); 
}


class MockActionHandler : public ActionRequestHandler
{
    public:
        MOCK_METHOD1(canHandleRequest, bool(HttpRequest req));
        MOCK_METHOD1(handleRequest, std::string(HttpRequest req));
};

class MockStaticHandler : public StaticFileRequestHandler
{
    public:
        MOCK_METHOD1(canHandleRequest, bool(HttpRequest req));
        MOCK_METHOD2(handleRequest, void(std::string* header, std::vector<char>* file));
};

TEST_F(SessionTest, NonHTTPRequestTest)
{
    char inputData[] = "Non-HTTP formatted input";
    strcpy(new_session.data_, inputData);
    std::string output = "HTTP/1.1 400 Bad Request\r\nContent-Length: 23\r\nContent-Type: text/plain\r\n\r\n400 Error: Bad request\n";
    new_session.sessionFileHandler = nullptr;
    new_session.handleRead(err, bytesTransferred);
    EXPECT_STREQ(new_session.response.c_str(), output.c_str());
}

TEST_F(SessionTest, ActionRequestTest)
{
    char inputData[] = "GET /echo HTTP/1.1\r\nContent-Type: text/plain\r\n\r\n";
    strcpy(new_session.data_, inputData);
    MockActionHandler m_actionHandler;
    MockStaticHandler m_staticHandler;
    new_session.sessionActionReqHandler = &m_actionHandler;
    new_session.sessionFileHandler = &m_staticHandler;
    EXPECT_CALL(m_staticHandler, canHandleRequest(_)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(m_actionHandler, canHandleRequest(_)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(m_staticHandler, handleRequest(_, _)).Times(0);
    EXPECT_CALL(m_actionHandler, handleRequest(_)).Times(1);
    
    new_session.handleRead(err, bytesTransferred);
}
// This test broke when I fixed the bug in session.cc, it needs to be fixed as well

TEST_F(SessionTest, StaticFileRequestTest)
{
    char inputData[] = "GET /static/somefile HTTP/1.1\r\nContent-Type: text/plain\r\n\r\n";
    strcpy(new_session.data_, inputData);
    MockActionHandler m_actionHandler;
    MockStaticHandler m_staticHandler;
    new_session.sessionActionReqHandler = &m_actionHandler;
    new_session.sessionFileHandler = &m_staticHandler;
    EXPECT_CALL(m_staticHandler, canHandleRequest(_)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(m_actionHandler, canHandleRequest(_)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(m_staticHandler, handleRequest(_, _)).Times(1);
    EXPECT_CALL(m_actionHandler, handleRequest(_)).Times(0);
    
    new_session.handleRead(err, bytesTransferred);
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
