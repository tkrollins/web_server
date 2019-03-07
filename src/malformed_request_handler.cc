#include "malformed_request_handler.h"
#include "http_response.h"

std::unique_ptr<RequestHandler> MalformedRequestHandler::create(const NginxConfig& config, const std::string& root_path)
{
    // TODO: set up attributes based on the config file
    MalformedRequestHandler malformedHandler;
    return std::make_unique<MalformedRequestHandler>(malformedHandler);
}

std::unique_ptr<HttpResponse> MalformedRequestHandler::HandleRequest(const HttpRequest &request)
{
    status = 400;
    std::string body = "400 Bad Request\r\n";
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                {"Content-Length", contentLengthStr}};
    HttpResponse res;
    res.setHttpResponse(status, headers, body);
    std::unique_ptr<HttpResponse> resPtr = std::make_unique<HttpResponse>(res);
    return resPtr;
}