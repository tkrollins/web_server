#include <handler_manager.h>
#include <http_response.h>
#include <http_request.h>
#include "gtest/gtest.h"

TEST(HandlerManagerTest, ErrorHandler)
{
    NginxConfig config;
    std::string root_path = "";
    HandlerManager manager;
    std::unique_ptr<RequestHandler> errorHandler = manager.createByName("400", config, root_path);

    HttpRequest req;
    std::unique_ptr<HttpResponse> res = errorHandler -> HandleRequest2(req);
    std::string expectedOutput = "HTTP/1.1 400 Bad Request\r\n"
                                 "Content-Length: 24\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "\r\n"
                                 "400 Error: Bad request\r\n";
    EXPECT_TRUE(res -> buildHttpResponse() == expectedOutput);
}




