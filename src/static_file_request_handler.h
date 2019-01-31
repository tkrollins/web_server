#ifndef STATIC_FILE_REQUEST_HANDLER_H
#define STATIC_FILE_REQUEST_HANDLER_H

#include "request_handler.h"
#include "fstream"


class StaticFileRequestHandler : public RequestHandler
{
public:
    bool canHandleRequest(HttpRequest req);
    StaticFileRequestHandler(std::unordered_map<std::string, std::string> staticPathMap);
    // std::vector<const_buffer> getResponseBuffer();

private:
    std::unordered_map<std::string, std::string> validURIMap;
    std::ifstream* getFileStream(std::string fileName);
};


#endif //STATIC_FILE_REQUEST_HANDLER_H
