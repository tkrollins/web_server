#include "gtest/gtest.h"
#include "submit_meme_handler.h"
#include "http_request.h"
#include "nginx_config.h"
#include <unordered_map>
#include "boost/filesystem.hpp"

using namespace std;

class SubmitMemeHandlerTest : public ::testing::Test {
protected:
	HttpRequest request;
    SubmitMemeHandler submitMemeHandler;
    std::string topText = "top";
    std::string bottomText = "bottom";
    std::string testID = "test_ID";
    std::string uriParam = "?update=";
    std::string error400 = "400 Error: Bad Request";
};

TEST_F(SubmitMemeHandlerTest, validSubmitMemeRequest)
{
    request.requestMethod = GET;
    request.requestURI = "meme/submit";

    // expected response should be the regular submit form
    std::string expectedResponseBody = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <title>Create a Meme!</title>\n</head>\n<h1>Create a Meme!</h1>\n<form action=\"/meme/create\" method=\"post\">\n    <select name=\"image\">\n        <option>Select a template...</option>\n        <option value=\"angry_lebron.jpg\">Angry LeBron</option>\n        <option value=\"bike_crash.jpg\">Bike Crash</option>\n    </select><br>\n    <input type=\"text\" name=\"top\" placeholder=\"Top text...\"><br>\n    <input type=\"text\" name=\"bottom\" placeholder=\"Bottom text...\"><br>\n    <input type=\"submit\" value=\"Create\">\n</form>\n</body>\n</html>";

    EXPECT_STREQ(expectedResponseBody.c_str(), submitMemeHandler.HandleRequest(request)->body.c_str());
}

TEST_F(SubmitMemeHandlerTest, updateMemeRequests)
{
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");
   
    database.Put(testID, "angry_lebron.jpg", MemeDB::IMAGE);
    database.Put(testID, topText, MemeDB::TOP_TEXT);
    database.Put(testID, bottomText, MemeDB::BOTTOM_TEXT);
    
    request.requestMethod = GET;
    std::string wrongTestID = "wrong_test_id";
    request.requestURI = "meme/submit" + uriParam + wrongTestID;	
	EXPECT_STREQ(error400.c_str(), submitMemeHandler.HandleRequest(request)->body.c_str());

	request.requestURI = "meme/submit" + uriParam + testID;
	std::string expectedResponseBody = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <title>Edit a Meme!</title>\n</head>\n<h1>Edit a Meme!</h1>\n<form action=\"/meme/create\" method=\"post\">\n  <select name=\"image\">\n    <option value=\"angry_lebron.jpg\">Angry LeBron</option>\n    <option value=\"bike_crash.jpg\">Bike Crash</option>\n  </select><br>\n  <input type=\"text\" name=\"top\" placeholder=\"Top text...\" value=\"" + topText + "\"><br>\n  <input type=\"text\" name=\"bottom\" placeholder=\"Bottom text...\" value=\"" + bottomText +"\"><br>\n  <input type=\"hidden\" name=\"update\" value=\"" + testID + "\">\n  <input type=\"submit\" value=\"Update\">\n</form>";
	EXPECT_STREQ(expectedResponseBody.c_str(), submitMemeHandler.HandleRequest(request)->body.c_str());

	database.Delete(testID, MemeDB::IMAGE);
	database.Delete(testID, MemeDB::TOP_TEXT);
	database.Delete(testID, MemeDB::BOTTOM_TEXT);
}