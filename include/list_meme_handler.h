#ifndef LIST_MEME_HANDLER_H
#define LIST_MEME_HANDLER_H

#include "request_handler.h"
#include "config_parser.h"
#include <memory>
#include "http_response.h"

class ListMemeHandler : public RequestHandler
{
public:
    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request) override;

    ListMemeHandler() {}
};


#endif //LIST_MEME_HANDLER_H
