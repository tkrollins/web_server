#ifndef ACTION_REQUEST_HANDLER_H
#define ACTION_REQUEST_HANDLER_H

#include "config_parser.h"
#include "http_request.h"

class ActionRequestHandler
{
public:
	ActionRequestHandler(std::unordered_map<ServerAction, std::string> serverActionMap);

    bool canHandleRequest(HttpRequest req);
    void clearVariables();
    std::string handleRequest(HttpRequest req);
    std::string echoRequest(HttpRequest req);
    std::string get400Response();

    std::string status;
    std::string URI;
    std::unordered_map<ServerAction, std::string> validActionMap;

};


#endif //ACTION_REQUEST_HANDLER_H
