#include <iostream>
#include <cassert>

#include "proxy_request_handler.h"

ProxyRequestHandler::ProxyRequestHandler(const NginxConfig &config){
    dest = "";
    std::cout << "destinated proxy address: " +  dest << std::endl;
}


std::unique_ptr<RequestHandler> ProxyRequestHandler::create(const NginxConfig& config, const std::string& root_path){
    ProxyRequestHandler proxyHandler(config);
    return std::make_unique<ProxyRequestHandler>(proxyHandler);
}

std::unique_ptr<HttpResponse> ProxyRequestHandler::HandleRequest(const HttpRequest &request)
{
    status = 200;
    std::string body = "Proxy Test\r\n";
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                {"Content-Length", contentLengthStr}};
    HttpResponse res;
    res.setHttpResponse(status, headers, body);
    std::unique_ptr<HttpResponse> resPtr = std::make_unique<HttpResponse>(res);
    return resPtr;
}
