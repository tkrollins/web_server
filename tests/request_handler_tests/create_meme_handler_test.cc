#include "create_meme_handler.h"
#include "gtest/gtest.h"
#include "http_request.h"
#include "meme_db.h"
#include "handler_manager.h"

TEST(CreateMemeHandlerTest, CreateWithPostRequest) {
    NginxConfig config;
    HandlerManager manager;
    std::unique_ptr<RequestHandler> createMemeHandler = manager.createByName("createMeme", config, "");
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");

    HttpRequest request;
    request.requestMethod = httpRequestEnum::POST;
    request.requestBody = "image=1&top=2&bottom=3";
    std::string generatedID = "MXRvcDoyYm90dG9tOjM="; // base64 encoding of "1top:2bottom:3"
    std::unique_ptr<HttpResponse> response = createMemeHandler -> HandleRequest(request);

    EXPECT_TRUE(database.Get(generatedID, MemeDB::IMAGE) == "1");
    EXPECT_TRUE(database.Get(generatedID, MemeDB::TOP_TEXT) == "2");
    EXPECT_TRUE(database.Get(generatedID, MemeDB::BOTTOM_TEXT) == "3");
    EXPECT_TRUE(response -> body.find(generatedID) != std::string::npos);

    bool successPic = database.Delete(generatedID, MemeDB::IMAGE);
    bool successTop = database.Delete(generatedID, MemeDB::TOP_TEXT);
    bool successBottom = database.Delete(generatedID, MemeDB::BOTTOM_TEXT);
    EXPECT_TRUE(successPic);
    EXPECT_TRUE(successTop);
    EXPECT_TRUE(successBottom);

}

TEST(CreateMemeHandlerTest, CreateWithGetRequest) {
    NginxConfig config;
    HandlerManager manager;
    std::unique_ptr<RequestHandler> createMemeHandler = manager.createByName("createMeme", config, "");
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");

    HttpRequest request;
    request.requestMethod = httpRequestEnum::GET;
    request.requestBody = "image=1&top=2&bottom=3";
    std::string generatedID = "MXRvcDoyYm90dG9tOjM="; // base64 encoding of "1top:2bottom:3"
    std::unique_ptr<HttpResponse> response = createMemeHandler -> HandleRequest(request);

    EXPECT_FALSE(database.Get(generatedID, MemeDB::IMAGE) == "1");
    EXPECT_FALSE(database.Get(generatedID, MemeDB::TOP_TEXT) == "2");
    EXPECT_FALSE(database.Get(generatedID, MemeDB::BOTTOM_TEXT) == "3");
    EXPECT_FALSE(response -> body.find(generatedID) != std::string::npos);

}

TEST(CreateMemeHandlerTest, MissingTopText) {
    NginxConfig config;
    HandlerManager manager;
    std::unique_ptr<RequestHandler> createMemeHandler = manager.createByName("createMeme", config, "");
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");

    HttpRequest request;
    request.requestBody = "image=1&bottom=3";
    std::unique_ptr<HttpResponse> response = createMemeHandler -> HandleRequest(request);

    EXPECT_TRUE(response -> body.find("404 not found") != std::string::npos);
}