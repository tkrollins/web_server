#include "error_request_handler.h"
#include "http_response.h"
#include <boost/log/trivial.hpp>
#include <memory>

bool ErrorRequestHandler::canHandleRequest(HttpRequest req)
{
    BOOST_LOG_TRIVIAL(debug) << "Invalid request URI: " << req.requestURI;
    return true;
}

void ErrorRequestHandler::handleRequest(std::string *response)
{
    status = 400;
    std::string body = "400 Error: Bad request\n";
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                {"Content-Length", contentLengthStr}};
    HttpResponse res;
    *response = res.buildHttpResponse(status, headers, body);
}

std::unique_ptr<RequestHandler> ErrorRequestHandler::create(const NginxConfig& config, const std::string& root_path)
{
    // TODO: set up attributes based on the config file
    ErrorRequestHandler errorHandler;
    return std::make_unique<ErrorRequestHandler>(errorHandler);
}

std::unique_ptr<HttpResponse> ErrorRequestHandler::HandleRequest2(const HttpRequest& request)
{
    status = 400;
    std::string body = "400 Error: Bad request\r\n";
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                {"Content-Length", contentLengthStr}};
    HttpResponse res;
    res.setHttpResponse(status, headers, body);
    std::unique_ptr<HttpResponse> resPtr = std::make_unique<HttpResponse>(res);
    return resPtr;
}