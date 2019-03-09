#include "list_meme_request_handler.h"
#include "http_response.h"
#include <boost/log/trivial.hpp>
#include "meme_db.h"
#include <memory>
#include <vector>

std::unique_ptr<RequestHandler> ListMemeHandler::create(const NginxConfig& config, const std::string& root_path)
{
    // TODO: set up attributes based on the config file
    ListMemeHandler listMemeHandler;
    return std::make_unique<ListMemeHandler>(listMemeHandler);
}

std::unique_ptr<HttpResponse> ListMemeHandler::HandleRequest(const HttpRequest &request)
{
    MemeDB database;

    status = 200;
    bool searchMode = false;
    // std::cout << "here01: " << request.requestURI << std::endl;
    std::vector<std::string> ids = database.getSortedIDs();
    std::string searchTarget;
    if (request.requestURI.find("q=") != std::string::npos) searchMode = true;
    if (searchMode)
    {
        searchTarget = request.requestURI.substr(request.requestURI.find("q=")+2);
        if (searchTarget.find("&") != std::string::npos){
            searchTarget = searchTarget.substr(0, searchTarget.find("&"));
        }
        // std::cout << "here02: " << searchTarget << std::endl;
        bool findFlag;
        for (auto it = ids.begin(); it != ids.end();)
        {
            std::string id = *it;
            findFlag = false;
            if (
                id.find(searchTarget) != std::string::npos ||
                database.Get(id, MemeDB::IMAGE).find(searchTarget) != std::string::npos ||
                database.Get(id, MemeDB::TOP_TEXT).find(searchTarget) != std::string::npos ||
                database.Get(id, MemeDB::BOTTOM_TEXT).find(searchTarget) != std::string::npos
            )findFlag = true;
            // std::cout << "here03: " << *it << ", ff = " << findFlag << std::endl;
            if (!findFlag) it = ids.erase(it++);
            else it++;
        }
    }

    std::string body = "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n    <meta charset=\"UTF-8\">\r\n    <title>Meme List</title>\r\n    <style>tr:nth-child(even) {background: #CCC}</style>\r\n</head>\r\n<body>\r\n<h1>Meme List</h1>\r\n";
    body += "<form method=\"get\">\r\n <input type=\"text\" name=\"q\" value=\"";
    if (searchMode) body += searchTarget;
    body += "\">\r\n <input type=\"submit\" value=\"Search\">\r\n</form>\r\n";
    if (searchMode && ids.size() != 0)
    {
        body += "Found ";
        body += std::to_string(ids.size());
        body += " results for <b>";
        body += searchTarget;
        body += "</b><br>\r\n";
    }
    else if (searchMode)
    {
        body += "No results found for <b>"; 
        body += searchTarget;
        body += "</b><br>\r\n";
    }
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

