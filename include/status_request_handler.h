#ifndef STATUS_REQUEST_HANDLER_H
#define STATUS_REQUEST_HANDLER_H

#include "request_handler.h"
#include "config_parser.h"
#include <memory>
#include "http_response.h"

class StatusRequestHandler : public RequestHandler
{
public:
	virtual bool canHandleRequest(HttpRequest req) override {}; // TODO: delete
	virtual void handleRequest(std::string* response) override {}; // TODO: delete

	StatusRequestHandler(const NginxConfig& config);
	~StatusRequestHandler() {};

    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request);
private:
    const NginxConfig* selfConfig;
    std::string retrieveStatusInfo(const NginxConfig& config);
};


#endif //STATUS_REQUEST_HANDLER_H
