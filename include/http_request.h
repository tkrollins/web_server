#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <unordered_map>
#include <string>

class HttpRequest
{
public:

    // These are the components of the request
    std::string requestMethod;
    std::string requestURI;
    std::string requestBody;

    // Map of header field and its corresponding value
    std::unordered_map<std::string, std::string> headerFields;

    // Parses the request, sets components, and return true if valid
    bool parseHttpRequest(std::string);

private:

    // Helper functions set each component within parseHttpRequest
    // Return true if successful
    bool setMethod(std::string);
    bool setURI(std::string);
    bool setBody(std::string);
    bool setHeaderFields(std::string);
};


#endif //HTTP_PARSER_H
