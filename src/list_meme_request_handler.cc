#include "list_meme_request_handler.h"
#include "http_response.h"
#include <boost/log/trivial.hpp>
#include "meme_db.h"
#include <memory>


std::unique_ptr<RequestHandler> ListMemeHandler::create(const NginxConfig& config, const std::string& root_path)
{
    // TODO: set up attributes based on the config file
    ListMemeHandler listMemeHandler;
    return std::make_unique<ListMemeHandler>(listMemeHandler);
}

std::unique_ptr<HttpResponse> ListMemeHandler::HandleRequest(const HttpRequest &request)
{
    MemeDB database;
    std::set<std::string> ids = database.getIDs();

    status = 200;
    std::string body = "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n    <meta charset=\"UTF-8\">\r\n    <title>Meme List</title>\r\n    <style>tr:nth-child(even) {background: #CCC}</style>\r\n</head>\r\n<body>\r\n<h1>Meme List</h1>\r\n";
    body += "<table style=\"width:100%\" border=\"1\">\r\n";
    body += "<tr>\r\n<th>Meme ID</th>\r\n<th>IMAGE</th>\r\n<th>TOP_TEXT</th>\r\n<th>BOTTOM_TEXT</th>\r\n</tr>\r\n";
    for (std::string id: ids)
    {
        body += "<tr>\r\n";
        body += "<td><a href=\"/meme/view?id=" + id + "\">" + id + "</a>" + "</td>\r\n";
        body += "<td>" + database.Get(id, MemeDB::IMAGE) + "</td>\r\n";
        body += "<td>" + database.Get(id, MemeDB::TOP_TEXT) + "</td>\r\n";
        body += "<td>" + database.Get(id, MemeDB::BOTTOM_TEXT) + "</td>\r\n";
        body += "</tr>\r\n";
    }
    body += "</table>\r\n";
    body += "</body>\r\n</html>";
    std::string contentLengthStr = std::to_string(body.length());
    std::map<std::string,std::string> headers { {"Content-Type", "text/html"},
                                                {"Content-Length", contentLengthStr}};
    HttpResponse res;
    res.setHttpResponse(status, headers, body);
    std::unique_ptr<HttpResponse> resPtr = std::make_unique<HttpResponse>(res);
    return resPtr;
}

