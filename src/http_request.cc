#include "http_request.h"
#include <queue>
#include <iostream>
#include <boost/algorithm/string.hpp>

bool HttpRequest::setMethod(std::string method)
{
    if (method.empty())
    {
      // handle null pointer exception
    }

    // TODO: implement function
    printf("http method: ");
    if (method.compare("GET") == 0)
    {
        this->requestMethod = GET;
        std::cout << method << std::endl;
        return true;
    }
    return false;
}

bool HttpRequest::setURI(std::string requestTarget)
{
    printf("request target: ");
    std::cout << requestTarget << std::endl;
    this->requestURI = requestTarget;
    return true;
}

bool HttpRequest::setVersion(std::string version)
{
    printf("version: ");
    std::cout << version << std::endl;
    this->httpVersion = version;
    return true;
}

bool HttpRequest::setBody(std::string body)
{
    this->requestBody = body;
    return true;
}

bool HttpRequest::setHeaderFields(std::string headerField, std::string headerValue)
{
    // case-insensitive compare the header field with legal header field strings 
    // (is there a cleaner way of doing this? Hashing won't work because the comparison
    // needs to be case-insensitive)
    printf("setting header field: ");
    std::cout << headerField << std::endl;
    printf("to: ");
    std::cout << headerValue << std::endl;
    if (boost::iequals(headerField, "Accept"))
    {
        this->headerFields[ACCEPT] = headerValue;
    }
    else if (boost::iequals(headerField, "Accept-Charset"))
    {
        this->headerFields[ACCEPT_CHARSET] = headerValue;
    }
    else if (boost::iequals(headerField, "Accept-Encoding"))
    {
        this->headerFields[ACCEPT_ENCODING] = headerValue;
    }
    else if (boost::iequals(headerField, "Accept-Language"))
    {
        this->headerFields[ACCEPT_LANGUAGE] = headerValue;
    }
    else if (boost::iequals(headerField, "Connection"))
    {
        this->headerFields[CONNECTION] = headerValue;
    }
    else if (boost::iequals(headerField, "Content-Length")) // this specifies the body's length in bytes
    {
        this->headerFields[CONTENT_LENGTH] = headerValue;
    }
    else if (boost::iequals(headerField, "Host"))
    {
        this->headerFields[HOST] = headerValue;
    }
    else if(boost::iequals(headerField, "User-Agent"))
    {
        this->headerFields[USER_AGENT] = headerValue;
    }
    return true;
}

// this method expects the HTTP request in string format and sets the fields in the HttpRequest object
// The input should only be the string representation of ONE single request
bool HttpRequest::parseHttpRequest(std::string requestString)
{
    if(requestString.empty())
    {
        // TODO: handle null ptr exception
    }    

    unparsedRequestString = requestString;

    printf("Parsing HTTP request \n");

    // divide up the requestString into lines
    std::vector<std::string> requestLines; // requestLines contains all parts of the request before the \r\n\r\n sequence
    int lineBeginning = 0; // index of the char at the beginning of each line
    int requestBodyStart;
    for (int i = 0; i < requestString.length(); i++)
    {
        // loop through each char in request, and if you see a new line,
        // push that line to the requestLines vector
        bool requestEndFound = false;
        if (requestString[i] == '\n' && requestString[i+1] == '\n' && !requestEndFound)
        {
            int lineLength = i - lineBeginning; // number of chars in the line
            std::string subString = requestString.substr(lineBeginning, lineLength); // the line
            requestLines.push_back(subString);
            requestBodyStart = i + 2; // the index of the request body's first char, if it exists
            break;
        }
        else if (requestString[i] == '\r' && // Windows HTTP request
                requestString[i+1] == '\n' && 
                requestString[i+2] == '\r' && 
                requestString[i+3] == '\n' &&
                !requestEndFound)
        {
            int lineLength = i - lineBeginning; // number of chars in the line
            std::string subString = requestString.substr(lineBeginning, lineLength); // the line
            requestLines.push_back(subString);
            requestBodyStart = i + 4; // the index of the request body's first char, if it exists
            break;
        }
        else if (requestString[i] == '\n')
        {
            int lineLength = i - lineBeginning; // number of chars in the line
            std::string subString = requestString.substr(lineBeginning, lineLength); // the line
            requestLines.push_back(subString);
            lineBeginning = i + 1; // update the index of the char in the next line
        }
    }

    bool methodFound = false;
    bool targetFound = false;
    bool versionFound = false;
    bool headersFound = false;

    for (int lineNumber = 0; lineNumber < requestLines.size(); ++lineNumber)
    {
        // first line is parsed out for the http method and version
        if (lineNumber == 0)
        {
            int targetBegin;
            int versionBegin;
            int methodBegin = 0;
            // loop through each character in the line
            for(std::string::iterator characterPtr = requestLines[lineNumber].begin(); 
                characterPtr != requestLines[lineNumber].end(); 
                characterPtr++)
            {
                // white space delimits the method, target and version. TODO: There's definitely a cleaner
                // way to do this via the string's member functions
                if (*characterPtr == ' ' || *characterPtr == '\n')
                {
                    if (!methodFound)
                    {
                        int nCharsInRequestMethod = characterPtr - requestLines[lineNumber].begin();

                        methodFound = setMethod(requestLines[lineNumber].substr(methodBegin, nCharsInRequestMethod));
                        targetBegin = methodBegin + nCharsInRequestMethod + 1;
                    }
                    else if (!targetFound)
                    {
                        int nCharsInRequestTarget = characterPtr - targetBegin - requestLines[lineNumber].begin();

                        targetFound = setURI(requestLines[lineNumber].substr(targetBegin, nCharsInRequestTarget));
                        versionBegin = targetBegin + nCharsInRequestTarget + 1;
                    }
                    // else if (!versionFound)
                    // {
                    //     int nCharsInVersion = characterPtr - versionBegin - requestLines[lineNumber].begin();
                    //     versionFound = setVersion(requestLines[lineNumber].substr(versionBegin, nCharsInVersion));
                    // }
                }
            }
            int nCharsInVersion = requestLines[lineNumber].end() - versionBegin - requestLines[lineNumber].begin();
            versionFound = setVersion(requestLines[lineNumber].substr(versionBegin, nCharsInVersion));
        }
        // if we are not on the first line, each subsequent line contains a header field
        else
        {
            std::string headerField;
            std::string headerValue;

            // TODO: error handling for case if you cannot find a colon
            int colonPosition = requestLines[lineNumber].find(":");
            headerField = requestLines[lineNumber].substr(0, colonPosition);
            headerValue = requestLines[lineNumber].substr(colonPosition + 2, requestLines[lineNumber].length()-colonPosition);
            headersFound = setHeaderFields(headerField, headerValue); 
        }
    }

    // if (headersFound)
    // {
    //     for (auto& x: this->headerFields)
    //         std::cout << x.first << ": " << x.second << std::endl;
    // }

    bool bodyFound = false;

    // TODO: sometimes, the content length isn't a numerical value (like in the readme)??
    // need to handle that case 
    // a body will exist if there is a specified content length in the headers (is this true?)
    // if (this->headerFields.find(CONTENT_LENGTH) != this->headerFields.end())
    // {
    //     // sorry about this line -- will clean it up. Just supposed to set the http body
    //     // attribute based on the content length
    //     bodyFound = setBody(requestString.substr(requestBodyStart, std::stoi(this->headerFields[CONTENT_LENGTH])));
    // }

    if (methodFound && targetFound && versionFound && headersFound)
        return true;
    else
    {
        return false;
    }
}
