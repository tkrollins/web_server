#include "view_meme_handler.h"
#include "http_response.h"
#include <boost/log/trivial.hpp>

using std::ifstream;
using std::ios;

ViewMemeHandler::ViewMemeHandler(
        const std::unordered_map<std::string, std::string> staticPathMap, std::string rootPath) :
        StaticFileRequestHandler()
{
    validURIMap = staticPathMap;
    root = rootPath;
    status = 200;
}

std::unique_ptr<RequestHandler> ViewMemeHandler::create(const NginxConfig& config, const std::string& root_path)
{
    ViewMemeHandler viewMemeHandler(config.getFlatParameters(), root_path);
    return std::make_unique<ViewMemeHandler>(viewMemeHandler);
}

void ViewMemeHandler::setURI(const HttpRequest &request)
{
	URI = request.requestURI;
}

// Parses the HttpRequest target to set the handler's meme id attribute
void ViewMemeHandler::setMemeID()
{
	int idStartPos = URI.find("?id=") + 4; // the meme id begins 4 characters after the '?'
	memeID = URI.substr(idStartPos, URI.length() - idStartPos);
}

void ViewMemeHandler::setFileName()
{
	MemeDB database;
	fileName = database.Get(memeID, MemeDB::IMAGE); // todo
}

// Looks up the meme id in the db and retrieves the meme information (path and text)
void ViewMemeHandler::setMemeText()
{
	MemeDB database;
    memeTextTop = database.Get(memeID, MemeDB::TOP_TEXT);
    memeTextBottom = database.Get(memeID, MemeDB::BOTTOM_TEXT);
}

// uses the the request handler's public attributes to construct an HTML containing the meme 
void ViewMemeHandler::buildMemeHTML()
{
	std::string styleElement = "<style>\n  body { display: inline-block; position: relative; }\n  span { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }\n  #top { top: 0; }\n  #bottom { bottom: 0; }\n</style>\n";
	std::string bodyElement = "<body>\n  <img src=\"" + pathToFile + "\">\n  <span id=\"top\">\"" + memeTextTop + "\"</span>\n  <span id=\"bottom\">\"" + memeTextBottom + "\"</span>\n</body>";
	memeAsHTML = styleElement + bodyElement;
}

std::unique_ptr<HttpResponse> ViewMemeHandler::HandleRequest(const HttpRequest &request)
{
	HttpResponse response;
	if (request.requestMethod != GET)
	{
		// handle non-get request by returning 400 error message
		status = 400;
		fileType = TXT;
		std::string body = "400 Error: Bad Request";
		setResponse(response, body);
		return std::make_unique<HttpResponse>(response);
	}
	
	setURI(request);
	setMemeID();
	setFileName();
	setPathToFile();

	if (doesFileExist() && !fileName.empty())
	{
		fileType = HTML;
		setMemeText();
		buildMemeHTML();
	    setResponse(response, memeAsHTML);
	}
	else
	{
	    BOOST_LOG_TRIVIAL(debug) << "Invalid File Request:" << pathToFile;
	    status = 404;
	    fileName = "404error.html";
	    pathToFile = root + "/static_files/404error.html";
	    std::string fileStr = fileToString(pathToFile);
	    setResponse(response, fileStr);
	}

    return std::make_unique<HttpResponse>(response);
}