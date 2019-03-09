#include "submit_meme_handler.h"
#include "http_response.h"
#include <boost/log/trivial.hpp>

using std::ifstream;
using std::ios;

std::unique_ptr<RequestHandler> SubmitMemeHandler::create(const NginxConfig& config, const std::string& root_path)
{
    SubmitMemeHandler submitMemeHandler;
    return std::make_unique<SubmitMemeHandler>(submitMemeHandler);
}

// Parses the HttpRequest target to set the handler's meme id attribute
bool SubmitMemeHandler::setUpdateID()
{
	std::string uriParameter = "?update=";
	size_t paramStartPos = URI.find(uriParameter);

	if (paramStartPos == std::string::npos) // if we weren't able to find the update parameter from the URI
	{
		return false;
	}
	else
	{
		size_t idStartPos =  paramStartPos + uriParameter.length(); // the meme id begins 8 characters after the '?'
		memeID = URI.substr(idStartPos, URI.length() - idStartPos);
		return true;
	}
}

// call this function AFTER you've attempted to set the meme parameters
bool SubmitMemeHandler::successfulIDLookups()
{
	// if Get(memeID) from the database returned empty strings for each meme parameter, then the meme id DNE
	if (fileName.empty() && memeTextTop.empty() && memeTextBottom.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

std::unique_ptr<HttpResponse> SubmitMemeHandler::generate400Error()
{
	HttpResponse response;
	status = 400;
	fileType = TXT;
	std::string body = "400 Error: Bad Request";
	setResponse(response, body);
	return std::make_unique<HttpResponse>(response);
}

std::unique_ptr<HttpResponse> SubmitMemeHandler::generateSubmitPage()
{
	HttpResponse response;
	fileType = HTML;
	std::string submitForm = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <title>Create a Meme!</title>\n</head>\n<h1>Create a Meme!</h1>\n<form action=\"/meme/create\" method=\"post\">\n    <select name=\"image\">\n        <option>Select a template...</option>\n        <option value=\"angry_lebron.jpg\">Angry LeBron</option>\n        <option value=\"bike_crash.jpg\">Bike Crash</option>\n    </select><br>\n    <input type=\"text\" name=\"top\" placeholder=\"Top text...\"><br>\n    <input type=\"text\" name=\"bottom\" placeholder=\"Bottom text...\"><br>\n    <input type=\"submit\" value=\"Create\">\n</form>\n</body>\n</html>";
	setResponse(response, submitForm);
	return std::make_unique<HttpResponse>(response);
}

std::unique_ptr<HttpResponse> SubmitMemeHandler::generateUpdatePage()
{
	HttpResponse response;
	fileType = HTML;
	std::string submitForm = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <title>Edit a Meme!</title>\n</head>\n<h1>Edit a Meme!</h1>\n<form action=\"/meme/create\" method=\"post\">\n  <select name=\"image\">\n    <option value=\"angry_lebron.jpg\">Angry LeBron</option>\n    <option value=\"bike_crash.jpg\">Bike Crash</option>\n  </select><br>\n  <input type=\"text\" name=\"top\" placeholder=\"Top text...\" value=\"" + memeTextTop + "\"><br>\n  <input type=\"text\" name=\"bottom\" placeholder=\"Bottom text...\" value=\"" + memeTextBottom +"\"><br>\n  <input type=\"hidden\" name=\"update\" value=\"" + memeID + "\">\n  <input type=\"submit\" value=\"Update\">\n</form>";
	setResponse(response, submitForm);
	return std::make_unique<HttpResponse>(response);
}

std::unique_ptr<HttpResponse> SubmitMemeHandler::HandleRequest(const HttpRequest &request)
{
	setURI(request);
	status = 200;
	
	if (request.requestMethod != GET)
	{
		// handle non-get request by returning 400 error message
		return generate400Error();
	}
	
	bool updateRequested = setUpdateID();
	if (updateRequested) // serve the update meme form
	{
		setFileName(); // set the file name of the meme ID
		setMemeText(); // set the top and bottom text of the meme ID

		// serve the update form if the ID exists
		if (successfulIDLookups())
		{
			return generateUpdatePage();
		}
		// otherwise, return a 400 error message
		else
		{
			return generate400Error();
		}
	}
	else // serve the regular meme submit form if update wasn't requested
	{
		return generateSubmitPage();
	}
}