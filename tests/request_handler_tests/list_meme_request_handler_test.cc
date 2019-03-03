#include "gtest/gtest.h"
#include "list_meme_request_handler.h"
#include "http_request.h"
#include "nginx_config.h"
#include <unordered_map>
#include "boost/filesystem.hpp"

using namespace std;

class ListMemeHandlerTest : public ::testing::Test {
protected:
    HttpRequest req;
};

TEST_F(ListMemeHandlerTest, BasicFunctionTest) {
    ListMemeHandler listMemeHandler;
    std::unique_ptr<HttpResponse> res = listMemeHandler.HandleRequest(req);
    EXPECT_TRUE(res->errorCode == 200);
    EXPECT_TRUE(res->body.find("Meme List") != std::string::npos);
}
