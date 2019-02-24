#ifndef PROXY_REQUEST_HANDLER_H
#define PROXY_REQUEST_HANDLER_H

#include "request_handler.h"
#include "config_parser.h"
#include "http_response.h"
#include <memory>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <istream>
#include <ostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class ProxyRequestHandler : public RequestHandler
{
public:
    ProxyRequestHandler(const std::unordered_map<std::string, std::string> staticPathMap, std::string destPath);
    virtual ~ProxyRequestHandler() {};
    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request);
//    virtual bool canHandleRequest(HttpRequest req) override {};
//    virtual void handleRequest(std::string* response) override {};

private:
    void setURI(std::string request_uri);

    std::unordered_map<std::string, std::string> validURIMap;
    std::string dest;
    std::string uriPath;
};

#endif //PROXY_REQUEST_HANDLER_H
