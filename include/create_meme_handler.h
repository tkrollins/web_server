#include "request_handler.h"
#include "config_parser.h"
#include <memory>
#include "http_response.h"

class CreateMemeHandler : public RequestHandler
{
public:
    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request) override;

private:
    bool parseRequestBody(const HttpRequest &request);
    std::unordered_map<std::string, std::string> params;
    std::string escape(const std::string& data);
};