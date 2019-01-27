#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "server.h"
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;

TEST(SessionTest, CheckResponse) 
{
    boost::asio::io_service io_service_;
    MockSession mockSess(io_service_);
	std::string input("Hello");
	std::string output(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 5\r\n"
		"\r\n"
		"Hello"
	);
	EXPECT_CALL(mockSess, render_response(input)).Times(1);
	std::string myOutput = mockSess.render_response(input);
    EXPECT_TRUE(myOutput == output) << "Output: " << mockSess.render_response(input);
}
int main(int argc, char** argv)
{
	
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}
