#include "request_handler.h"
#include "config_parser.h"
#include "http_response.h"

class MalformedRequestHandler : public RequestHandler
{
public:
    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request) override;
};