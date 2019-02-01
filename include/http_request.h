#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <unordered_map>
#include <string>

#include "http_request_enums.h"

using namespace httpRequestEnum;

class HttpRequest
{
public:

    // These are the components of the request
    httpMethod requestMethod;
    std::string httpVersion;
    std::string requestURI;
    std::string requestBody;

    // Map of header field and its corresponding value
    std::unordered_map<httpHeaderFields, std::string> headerFields;

    // Parses the request, sets components, and return true if valid
    bool parseHttpRequest(std::string requestString);

private:

    // Helper functions set each component within parseHttpRequest
    // Return true if successful
    bool setMethod(std::string);
    bool setURI(std::string);
    bool setVersion(std::string);
    bool setHeaderFields(std::string, std::string);
    bool setBody(std::string);
};


#endif //HTTP_PARSER_H
