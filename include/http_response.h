#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <unordered_map>
#include <iostream>
#include <map>

class HttpResponse
{
public:
    // Builds a valid HTTP response of the status, headers, and body
    // Only the status is required the headers and body are optional and have empty default values
    // Returns the response as a string.  Returns empty string if invalid inputs
    std::string buildHttpResponse(std::string status, std::map<std::string,std::string> headers = {{}},
                                  std::string body = "");

private:
    // Helper functions to build the response within buildHttpResponse()
    std::string buildStatus(std::string);
    std::string buildHeaders(std::string, std::string);
    std::string buildBody(std::string);
};


#endif //HTTP_RESPONSE_H
