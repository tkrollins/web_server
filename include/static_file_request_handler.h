#ifndef STATIC_FILE_REQUEST_HANDLER_H
#define STATIC_FILE_REQUEST_HANDLER_H

#include "request_handler.h"
#include "fstream"


class StaticFileRequestHandler : public RequestHandler
{
public:
    virtual bool canHandleRequest(HttpRequest req) override;
    StaticFileRequestHandler() {}; // for testing
    StaticFileRequestHandler(std::unordered_map<std::string, std::string> staticPathMap, std::string rootPath);
    virtual void handleRequest(std::string* header, std::vector<char>* file) override;

//private:

    enum FileType
    {
        TXT = 0,
        HTML = 1,
        GIF = 2,
        JPEG = 3
    };

    std::unordered_map<std::string, std::string> validURIMap;
    std::string root;
    std::string fileName;
    std::string pathToFile;
    std::string URI;
    FileType fileType;
    bool fileExists;


    void setFileType();
    void setURIAndFileName(std::string uriWithFileName);
    void setPathToFile();
    void clearVariables();

    // https://stackoverflow.com/questions/25308676/send-file-via-boost-asio-tcp-how-to-stream-char-array-to-socket
    std::vector<char> fileToVector(const std::string &name);
    std::string createResponseHeader(unsigned contentLength);
    bool doesFileExist();
    void initRequestVariables();

};


#endif //STATIC_FILE_REQUEST_HANDLER_H
