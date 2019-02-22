#ifndef PROXY_REQUEST_HANDLER_H
#define PROXY_REQUEST_HANDLER_H

#include "request_handler.h"
#include "config_parser.h"
#include "http_response.h"
#include <memory>
#include <boost/log/trivial.hpp>

class ProxyRequestHandler : public RequestHandler
{
public:
    ProxyRequestHandler(const NginxConfig& config);
    virtual ~ProxyRequestHandler() {};
    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request);
    virtual bool canHandleRequest(HttpRequest req) override {};
    virtual void handleRequest(std::string* response) override {};

private:
    std::string dest;
};

#endif //PROXY_REQUEST_HANDLER_H
