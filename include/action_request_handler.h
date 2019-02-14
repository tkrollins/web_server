#ifndef ACTION_REQUEST_HANDLER_H
#define ACTION_REQUEST_HANDLER_H

#include "config_parser.h"
#include "http_request.h"
#include "http_response.h"
#include "request_handler.h"
#include "config_parser.h"
#include <memory>

class ActionRequestHandler : public RequestHandler
{
public:
	static RequestHandler* create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest2(const HttpRequest& request);

	ActionRequestHandler(std::unordered_map<ServerAction, std::string> serverActionMap);
    ActionRequestHandler() = default; // for testing
    virtual bool canHandleRequest(HttpRequest req) override;
    void clearVariables();
    virtual void handleRequest(std::string* response) override;
    std::string echoRequest();

    std::string URI;
    std::unordered_map<ServerAction, std::string> validActionMap;
    std::string body;

};


#endif //ACTION_REQUEST_HANDLER_H
