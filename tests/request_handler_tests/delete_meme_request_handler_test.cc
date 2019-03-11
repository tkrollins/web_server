#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "delete_meme_request_handler.h"
#include "http_request.h"
#include "nginx_config.h"
#include "boost/filesystem.hpp"
#include "meme_db.h"

using namespace std;
using ::testing::AtLeast;

class DeleteMemeHandlerTest : public ::testing::Test {
protected:
    HttpRequest req;
};

TEST_F(DeleteMemeHandlerTest, BasicFunctionTest) {
    DeleteMemeHandler deleteMemeHandler;
    std::unique_ptr<HttpResponse> res = deleteMemeHandler.HandleRequest(req);
    EXPECT_TRUE(res->errorCode == 200);
    EXPECT_TRUE(res->body.find("Delete Meme") != std::string::npos);
}

TEST_F(DeleteMemeHandlerTest, BadRequestTest) {
    DeleteMemeHandler deleteMemeHandler;
    req.requestURI = "/meme/delete?it=asdf";
    std::unique_ptr<HttpResponse> res = deleteMemeHandler.HandleRequest(req);
    EXPECT_TRUE(res->errorCode == 200);
    EXPECT_TRUE(res->body.find("Please specify ID") != std::string::npos);
}

