#include "create_meme_handler.h"
#include "gtest/gtest.h"
#include "http_request.h"
#include "meme_db.h"
#include "handler_manager.h"
#include <set>
#include <thread>

void createMemes(int threadCount)
{
    NginxConfig config;
    HandlerManager manager;
    std::unique_ptr<RequestHandler> createMemeHandler = manager.createByName("createMeme", config, "");

    // Add 100 memes to the database
    for(int i = 0; i < threadCount; i++)
    {
        HttpRequest request;
        request.requestMethod = httpRequestEnum::POST;
        request.requestBody = "image=1&top=2&bottom=3";
        std::unique_ptr<HttpResponse> response = createMemeHandler -> HandleRequest(request);
    }
}

TEST(CreateMemeHandlerTest, TestThreadSafety)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");

    // Each thread will add 5000 memes to database
    // Idea is they should be storing memes simultaneously
    std::thread thread1(createMemes, 500);
    std::thread thread2(createMemes, 500);

    thread1.join();
    thread2.join();

    // No IDs should have gotten lost or overwritten
    EXPECT_TRUE(database.getIDs().size() == 1000);
    database.clear();
}


TEST(CreateMemeHandlerTest, CreateWithPostRequest) {
    NginxConfig config;
    HandlerManager manager;
    std::unique_ptr<RequestHandler> createMemeHandler = manager.createByName("createMeme", config, "");
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");

    HttpRequest request;
    request.requestMethod = httpRequestEnum::POST;
    request.requestBody = "image=1&top=2&bottom=3";
    std::unique_ptr<HttpResponse> response = createMemeHandler -> HandleRequest(request);
    std::string generatedID = *(database.getIDs().begin());


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
    std::unique_ptr<HttpResponse> response = createMemeHandler -> HandleRequest(request);

    EXPECT_TRUE(response -> body.find("404 not found") != std::string::npos);

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
    database.clear();
}