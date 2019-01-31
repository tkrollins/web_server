#include "static_file_request_handler.h"

using std::ifstream;
using std::ios;

StaticFileRequestHandler::StaticFileRequestHandler(
        std::unordered_map<std::string, std::string> staticPathMap) :
        RequestHandler()
{
    validURIMap = staticPathMap;
}

bool StaticFileRequestHandler::canHandleRequest(HttpRequest req)
{
    // TODO: implement
    return true;
}

ifstream* StaticFileRequestHandler::getFileStream(std::string fileName)
{
    ifstream* staticFile = new ifstream(fileName.c_str(), ios::in|ios::binary);
    return staticFile;
}
 /*
std::vector<const_buffer> StaticFileRequestHandler::getResponseBuffer()
{
    // TODO: implement once HttpRequest api settled
}
 */
