#include "error_request_handler.h"
#include "http_response.h"
#include <boost/log/trivial.hpp>

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