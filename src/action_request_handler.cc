#include "action_request_handler.h"
#include "http_response.h"
#include <boost/log/trivial.hpp>

ActionRequestHandler::ActionRequestHandler(
        std::unordered_map<ServerAction, std::string> serverActionMap) :
        RequestHandler()
{
    validActionMap = serverActionMap;
    status = 200;
}


void ActionRequestHandler::clearVariables()
{
    URI = "";
    status = 200;
}

bool ActionRequestHandler::canHandleRequest(HttpRequest req)
{    
    // TODO: It would be nice if we could redefine the action map to map
    // the other way around, then this one-liner would work. Otherwise,
    // we would need to add a new OR condition in this if statement every time we add a new action
    if (validActionMap[ACTION_ECHO] == req.getRequestURI())
    {
        clearVariables();
        URI = req.getRequestURI();
        body = req.unparsedRequestString;
        return true;
    }
    else
    {
        return false;
    }
}

std::string ActionRequestHandler::echoRequest()
{
    // should it return the raw request back or an http response containing the request in the body?
    // I assume we do the latter
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                {"Content-Length", contentLengthStr}};

    HttpResponse response;
    return response.buildHttpResponse(status, headers, body);
}

void ActionRequestHandler::handleRequest(std::string* response)
{
        BOOST_LOG_TRIVIAL(trace) << "ECHO Action Triggered";
        *response = echoRequest();
}

std::unique_ptr<RequestHandler> ActionRequestHandler::create(const NginxConfig& config, const std::string& root_path)
{

}

std::unique_ptr<HttpResponse> ActionRequestHandler::HandleRequest2(const HttpRequest& request)
{


}

