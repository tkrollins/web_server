#include "create_meme_handler.h"
#include "gtest/gtest.h"
#include "http_request.h"
#include "meme_db.h"
#include "handler_manager.h"
#include <set>
#include "base64.h"
#include <thread>

void createMemes(std::set<std::string>* IDs, int i)
{
    NginxConfig config;
    HandlerManager manager;
    std::unique_ptr<RequestHandler> createMemeHandler = manager.createByName("createMeme", config, "");

    int stop = i + 100;

    // Add 100 memes to the database
    for(; i<stop; i++)
    {
        char buff[50];
        sprintf(buff, "image=%d&top=%d&bottom=%d", i, i+1, i+2);

        HttpRequest request;
        request.requestMethod = httpRequestEnum::POST;
        request.requestBody = buff;

        std::unique_ptr<HttpResponse> response = createMemeHandler -> HandleRequest(request);

        char buff2[50];
        sprintf(buff2, "%dtop:%dbottom:%d", i, i+1, i+2);
        std::string paramsString = buff2;
        std::string id = base64_encode(reinterpret_cast<const unsigned char*>(paramsString.c_str()), paramsString.length());

        // Keep track of meme ids added to database
        IDs->insert(id);
    }
}

bool compareIDs(const std::set<std::string>& subsetIDs, std::set<std::string>& IDs)
{
    for(auto& id : subsetIDs)
    {
        if(IDs.count(id) == 0)
        {
            return false;
        }
    }
    return true;
}

TEST(CreateMemeHandlerTest, TestThreadSafety)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");

    std::set<std::string> IDs_1;
    std::set<std::string> IDs_2;
    std::set<std::string> IDs_all;

    // Each thread will add 100 memes to database
    // Idea is they should be storing memes simultaneously
    std::thread thread1(createMemes, &IDs_1, 1);
    std::thread thread2(createMemes, &IDs_2, 200);

    thread1.join();
    thread2.join();

    // All meme IDs stored in database
    IDs_all = database.getIDs();

    // No IDs should have gotten lost or overwritten
    EXPECT_TRUE(compareIDs(IDs_1, IDs_all));
    EXPECT_TRUE(compareIDs(IDs_2, IDs_all));

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