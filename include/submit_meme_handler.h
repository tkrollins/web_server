#ifndef SUBMIT_MEME_HANDLER_H
#define SUBMIT_MEME_HANDLER_H

// #include "meme_db.h"
#include "view_meme_handler.h"
#include "http_response.h"
#include "fstream"
#include "config_parser.h"
#include <memory>

// need parent class's file path setting, valid file checking, and status recording features
class SubmitMemeHandler : public ViewMemeHandler 
{
public:
    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request);
    SubmitMemeHandler() {};

    bool setUpdateID();
    bool successfulIDLookups();
    std::unique_ptr<HttpResponse> generate400Error();
    std::unique_ptr<HttpResponse> generateSubmitPage();
    std::unique_ptr<HttpResponse> generateUpdatePage();
};


#endif //SUBMIT_MEME_HANDLER_H