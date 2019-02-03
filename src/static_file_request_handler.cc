#include "static_file_request_handler.h"
#include "http_response.h"

using std::ifstream;
using std::ios;

StaticFileRequestHandler::StaticFileRequestHandler(
        std::unordered_map<std::string, std::string> staticPathMap, std::string rootPath) :
        RequestHandler()
{
    validURIMap = staticPathMap;
    root = rootPath;
    status = "200";
}

void StaticFileRequestHandler::setFileType()
{
    if(fileName.find(".txt") != std::string::npos)
    {
        fileType = TXT;
    }
    else if(fileName.find(".html") != std::string::npos)
    {
        fileType = HTML;
    }
    else if(fileName.find(".gif") != std::string::npos)
    {
        fileType = GIF;
    }
    else if(fileName.find(".jpg") != std::string::npos)
    {
        fileType = JPEG;
    }
}

void StaticFileRequestHandler::clearVariables()
{
    fileName = "";
    pathToFile = "";
    URI = "";
    status = "200";
}

void StaticFileRequestHandler::setURIAndFileName(std::string uriWithFileName)
{
    if(uriWithFileName.length() > 1)
    {
        std::string::iterator it = uriWithFileName.begin();
        if (*it == '/')
        {
            ++it;
            URI.push_back('/');
            for (; *it != '/' && it != uriWithFileName.end(); ++it)
            {
                URI.push_back(*it);
            }
            if (it != uriWithFileName.end())
            {
                ++it;
                for (; it != uriWithFileName.end(); ++it)
                {
                    fileName.push_back(*it);
                }
            }
        }
    }
}

void StaticFileRequestHandler::setPathToFile()
{
    std::string path = root + validURIMap[URI];
    pathToFile = path + '/' + fileName;
}

bool StaticFileRequestHandler::doesFileExist()
{
    ifstream fileToServe(pathToFile);
    return fileToServe.good();
}

bool StaticFileRequestHandler::canHandleRequest(HttpRequest req)
{
    clearVariables();
    setURIAndFileName(req.requestURI);
    return (bool)validURIMap.count(URI);
}

std::string StaticFileRequestHandler::createResponseHeader(unsigned contentLength)
{
    std::string contentLengthStr(std::to_string(contentLength));
    std::string mimeType;
    switch (fileType)
    {
        case TXT:
            mimeType = "text/plain";
            break;
        case HTML:
            mimeType = "text/html";
            break;
        case GIF:
            mimeType = "image/gif";
            break;
        case JPEG:
            mimeType = "image/jpeg";
            break;
    }
    std::map<std::string,std::string> headers { {"Content-Type", mimeType},
                                                {"Content-Length", contentLengthStr}};

    HttpResponse response;

    return response.buildHttpResponse(status, headers);
}

void StaticFileRequestHandler::initRequestVariables()
{
    setPathToFile();
    if(!doesFileExist())
    {
        fileName = "404error.html";
        pathToFile = root + "/static_files/404error.html";
        status = "404";
    }
    setFileType();
}

// https://stackoverflow.com/questions/25308676/send-file-via-boost-asio-tcp-how-to-stream-char-array-to-socket
std::vector<char> StaticFileRequestHandler::fileToVector(const std::string &name)
{
    using namespace std;
    ifstream fl(name);
    fl.seekg( 0, ios::end );
    size_t len = fl.tellg();
    vector<char> ret(len);
    fl.seekg(0, ios::beg);
    if (len)
        fl.read(&ret[0], len);
    fl.close();
    // returns file as vector<char>
    return move(ret);
}

void StaticFileRequestHandler::handleRequest(std::string* header, std::vector<char>* file)
{
    initRequestVariables();

    std::vector<char> fileCharVector = fileToVector(pathToFile);
    *header = createResponseHeader(fileCharVector.size());
    file->resize(fileCharVector.size());
    *file = fileCharVector;
}

