#include "action_request_handler.h"
#include "http_response.h"

ActionRequestHandler::ActionRequestHandler(
        std::unordered_map<ServerAction, std::string> serverActionMap) 
{
    validActionMap = serverActionMap;
    status = "200";
}


void ActionRequestHandler::clearVariables()
{
    URI = "";
    status = "200";
}

bool ActionRequestHandler::canHandleRequest(HttpRequest req)
{    
    // TODO: It would be nice if we could redefine the action map to map
    // the other way around, then this one-liner would work. Otherwise,
    // we would need to add a new OR condition in this if statement every time we add a new action
    if (validActionMap[ACTION_ECHO].compare(req.getRequestURI()) == 0)
    {
        clearVariables();
        URI = req.getRequestURI();
        return true;
    }
    else
    {
        return false;
    }
}

std::string ActionRequestHandler::echoRequest(HttpRequest req)
{
    // should it return the raw request back or an http response containing the request in the body?
    // I assume we do the latter
    std::string body = req.unparsedRequestString;
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                {"Content-Length", contentLengthStr}};

    HttpResponse response;
    return response.buildHttpResponse(status, headers, body);
}

std::string ActionRequestHandler::get400Response()
{
    status = "400";
    std::string body = "400 Error: Bad request\n";
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                {"Content-Length", contentLengthStr}};
    HttpResponse response;
    return response.buildHttpResponse(status, headers, body);
}

std::string ActionRequestHandler::handleRequest(HttpRequest req)
{
    // TODO: add more cases to accomodate other actions that will be added
    // to server config
    if (validActionMap[ACTION_ECHO].compare(req.getRequestURI()) == 0)
    {
        return echoRequest(req);
    }
    else
    {
        return get400Response();
    }
}

