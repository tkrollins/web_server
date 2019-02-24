#include "gtest/gtest.h"
#include "proxy_request_handler.h"
#include "http_request.h"
#include "nginx_config.h"
#include <unordered_map>
#include "boost/filesystem.hpp"

using namespace std;

class ProxyRequestHandlerTest : public ::testing::Test {
protected:
    unordered_map<string, string> pathMap { {"dest", "www.rubberducks.cs130.org"} };

    HttpRequest req;

};

// TEST_F(ProxyRequestHandlerTest, SetURI) {
//     ProxyRequestHandler proxyHandler(pathMap, "../..");
//     req.requestURI = "/ucla";
//     proxyHandler.setURI(req.requestURI);

//     EXPECT_TRUE(proxyHandler.uriPath == "/");
//     cout << proxyHandler.uriPath << endl;
// }

TEST_F(ProxyRequestHandlerTest, ProxyNotFound) {

    ProxyRequestHandler proxyHandler(pathMap, "../..");
    req.requestURI = "/rubberducks/pathnotexist";
    std::unique_ptr<HttpResponse> res = proxyHandler.HandleRequest(req);
    EXPECT_TRUE(res->errorCode == 404);
}

TEST_F(ProxyRequestHandlerTest, ProxySuccessful) {

    ProxyRequestHandler proxyHandler(pathMap, "../..");
    req.requestURI = "/rub/echo";
    std::unique_ptr<HttpResponse> res = proxyHandler.HandleRequest(req);
    cout << res -> errorCode << endl;
    EXPECT_TRUE(res->errorCode == 200);
}