#ifndef VIEW_MEME_HANDLER_H
#define VIEW_MEME_HANDLER_H

#include "meme_db.h"
#include "static_file_request_handler.h"
#include "http_response.h"
#include "fstream"
#include "config_parser.h"
#include <memory>

// need parent class's file path setting, valid file checking, and status recording features
class ViewMemeHandler : public StaticFileRequestHandler 
{
public:
    static std::unique_ptr<RequestHandler> create(const NginxConfig& config, const std::string& root_path);
    std::unique_ptr<HttpResponse> HandleRequest(const HttpRequest &request);

    ViewMemeHandler() = default; // for testing
    ViewMemeHandler(const std::unordered_map<std::string, std::string> staticPathMap, std::string rootPath);
    
    // stores the HttpRequest URI in URI attribute
    void setURI(const HttpRequest &request);    

    // set the handler's memeID attribute by parsing the URI attribute
    void setMemeID();

    // sets the location of the handler that will serve the requested meme image
    void setMemeURI();

    // Looks up the meme id in the db and retrieves the meme information (path and text)
    void setFileName();
    void setMemeText();

    // uses the the request handler's public attributes to construct an HTML containing the meme 
    void buildMemeHTML();

    std::string memeID;
    std::string memeURI;
    std::string memeTextTop;
    std::string memeTextBottom;
    std::string memeAsHTML;
};


#endif //VIEW_MEME_HANDLER_H