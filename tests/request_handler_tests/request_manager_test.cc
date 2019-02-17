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

TEST(HandlerManagerTest, StaticHandler)
{
    // create the handler
    NginxConfig config;
    config.addFlatParam("/static", "/static_files/some_files");
    std::string root_path = "../..";
    HandlerManager manager;
    std::unique_ptr<RequestHandler> staticHandler = manager.createByName("static", config, root_path);

    // create fake request
    HttpRequest request;
    request.requestURI = "/static/home.html";

    // call handleRequest and verify the response
    std::unique_ptr<HttpResponse> response = staticHandler -> HandleRequest2(request);
    EXPECT_TRUE(response -> errorCode == 200);
    EXPECT_TRUE(response -> mimeType == "text/html");

    // make sure the body contains "THIS IS THE HOME PAGE OF BIGBEAR"
    EXPECT_TRUE(response -> body.find("THIS IS THE HOME PAGE OF BIGBEAR") != std::string::npos);
    std::cerr << response -> body;
}




