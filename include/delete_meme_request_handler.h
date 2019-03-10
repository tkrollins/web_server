#ifndef DELETE_MEME_REQUEST_HANDLER_H
#define DELETE_MEME_REQUEST_HANDLER_H

#include "request_handler.h"
#include "config_parser.h"
#include <memory>
#include "http_response.h"

class DeleteMemeHandler : public RequestHandler
{
public:
    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request) override;

    DeleteMemeHandler() {}
};


#endif //DELETE_MEME_REQUEST_HANDLER_H
