#include "gtest/gtest.h"
#include "view_meme_handler.h"
#include "http_request.h"
#include "nginx_config.h"
#include <unordered_map>
#include "boost/filesystem.hpp"

using namespace std;

class ViewMemeHandlerTest : public ::testing::Test {
protected:
    unordered_map<string, string> pathMap { {"root", "static_files/memes/images"}, {"memeURI", "/memes"} };
    std::string root_path = "/usr/src/projects/bigbear";

    HttpRequest request;

    std::string expectedPathToFile = root_path + '/' + pathMap["root"] + "/angry_lebron.jpg";

    std::string styleElement = "<style>\n  body { display: inline-block; position: relative; }\n  span { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }\n  #top { top: 0; }\n  #bottom { bottom: 0; }\n</style>\n";
	std::string bodyElement = "<body>\n  <img src=\"/memes/angry_lebron.jpg\">\n  <span id=\"top\">top</span>\n  <span id=\"bottom\">bottom</span>\n</body>";
	std::string expectedResponseBody = styleElement + bodyElement;
};

TEST_F(ViewMemeHandlerTest, validViewMemeRequest)
{
    request.requestMethod = GET;
    request.requestURI = "meme/view?id=test_id";

    ViewMemeHandler viewMemeHandler(pathMap, root_path);
    MemeDB database("/usr/src/projects/bigbear/build/tmp_db");

    std::string testID = "test_id";
    
    database.Put(testID, "angry_lebron.jpg", MemeDB::IMAGE);
    database.Put(testID, "top", MemeDB::TOP_TEXT);
    database.Put(testID, "bottom", MemeDB::BOTTOM_TEXT);
	
	viewMemeHandler.HandleRequest(request);

	database.Delete(testID, MemeDB::IMAGE);
	database.Delete(testID, MemeDB::TOP_TEXT);
	database.Delete(testID, MemeDB::BOTTOM_TEXT);

	EXPECT_STREQ(viewMemeHandler.URI.c_str(), request.requestURI.c_str());
	EXPECT_STREQ(viewMemeHandler.memeID.c_str(), testID.c_str());
	EXPECT_STREQ(viewMemeHandler.fileName.c_str(), "angry_lebron.jpg");
	EXPECT_EQ(viewMemeHandler.fileType, StaticFileRequestHandler::HTML);
	EXPECT_STREQ(viewMemeHandler.memeTextTop.c_str(), "top");
	EXPECT_STREQ(viewMemeHandler.memeTextBottom.c_str(), "bottom");
	EXPECT_STREQ(viewMemeHandler.pathToFile.c_str(), expectedPathToFile.c_str());
	EXPECT_STREQ(viewMemeHandler.memeAsHTML.c_str(), expectedResponseBody.c_str());

}