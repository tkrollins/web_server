#include "health_request_handler.h"
#include "http_response.h"

std::unique_ptr<RequestHandler> HealthRequestHandler::create(const NginxConfig& config, const std::string& root_path)
{
    // TODO: set up attributes based on the config file
    HealthRequestHandler healthHandler;
    return std::make_unique<HealthRequestHandler>(healthHandler);
}

std::unique_ptr<HttpResponse> HealthRequestHandler::HandleRequest(const HttpRequest &request)
{
    status = 200;
    std::string body = "OK\r\n";
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                {"Content-Length", contentLengthStr}};
    HttpResponse res;
    res.setHttpResponse(status, headers, body);
    std::unique_ptr<HttpResponse> resPtr = std::make_unique<HttpResponse>(res);
    return resPtr;
}
