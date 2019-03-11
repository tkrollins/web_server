#include "delete_meme_request_handler.h"
#include "http_response.h"
#include <boost/log/trivial.hpp>
#include "meme_db.h"
#include <memory>

std::unique_ptr<RequestHandler> DeleteMemeHandler::create(const NginxConfig& config, const std::string& root_path)
{
    // TODO: set up attributes based on the config file
    DeleteMemeHandler deleteMemeHandler;
    return std::make_unique<DeleteMemeHandler>(deleteMemeHandler);
}

std::unique_ptr<HttpResponse> DeleteMemeHandler::HandleRequest(const HttpRequest &request)
{
    MemeDB database;
    std::string body = "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n    <meta charset=\"UTF-8\">\r\n    <title>Delete Meme</title>\r\n</head>\r\n";
    if (request.requestURI.find("id=") != std::string::npos)
    {
        std::string deleteTarget;
        deleteTarget = request.requestURI.substr(request.requestURI.find("id=")+3);
        if (deleteTarget.find("&") != std::string::npos){
            deleteTarget = deleteTarget.substr(0, deleteTarget.find("&"));
        }
        if(database.deleteByID(deleteTarget))
        {
            body += "<body>Delete Success</body>";
        }
        else
        {
            body += "<body>ID not found</body>";
        }

    }
    else
    {
        body += "<body>Please specify ID</body>";
    }

    status = 200;
    HttpResponse res;
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/html"},
                                                {"Content-Length", contentLengthStr}};
    res.setHttpResponse(status, headers, body);
    std::unique_ptr<HttpResponse> resPtr = std::make_unique<HttpResponse>(res);
    return resPtr;
}

