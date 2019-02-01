#include "http_request.h"
#include <queue>
#include <iostream>
bool HttpRequest::setMethod(std::string httpMethod)
{
    if (httpMethod.empty())
    {
      // handle null pointer exception
    }

    // TODO: implement function
    printf("http method: ");
    if (httpMethod.compare("GET") == 0)
    {
        this->requestMethod = GET;
        std::cout << httpMethod << std::endl;
        return true;
    }
    return false;
}

bool HttpRequest::setURI(std::string)
{
    // TODO: implement function
    return true;
}

bool HttpRequest::setBody(std::string)
{
    // TODO: implement function
    return true;
}

bool HttpRequest::setHeaderFields(std::string)
{
    // TODO: implement function
    return true;
}

bool HttpRequest::parseHttpRequest(std::string requestString)
{
    printf("Parsing HTTP request \n");
    // divide up the requestString into lines
    std::vector<std::string> requestLines;
    int lineBeginning = 0; // index of the char at the beginning of each line
    for (int i = 0; i < requestString.length(); i++)
    {
        // loop through each char in request, and if you see a new line,
        // push that line to the requestLines vector
        if (requestString[i] == '\n')
        {
            int lineLength = i - lineBeginning; // number of chars in the line
            std::string subString = requestString.substr(lineBeginning, lineLength); // the line
            requestLines.push_back(subString);
            lineBeginning = i + 1; // update the index of the char in the next line
        }
    }

    // loop through each line
    bool methodFound = false;
    bool targetFound = false;
    bool versionFound = false;
    for (int lineNumber = 0; lineNumber < requestLines.size(); ++lineNumber)
    {
        // first line is parsed out for the http method and version
        if (lineNumber == 0)
        {
            std::string::iterator targetBeginPtr;
            std::string::iterator versionBeginPtr;

            int methodStart = 0;
            for(std::string::iterator characterPtr = requestLines[lineNumber].begin(); 
                characterPtr != requestLines[lineNumber].end(); 
                characterPtr++)
            {
                if (*characterPtr == ' ')
                {
                    if (!methodFound)
                    {
                        printf("method found\n");
                        int nCharsInRequestMethod = characterPtr - requestLines[lineNumber].begin();
                        methodFound = setMethod(requestLines[lineNumber].substr(0, nCharsInRequestMethod));
                        targetBeginPtr = characterPtr + 1;
                    }
                    else if (!targetFound)
                    {
                        int nCharsInRequestTarget = &characterPtr - &targetBeginPtr;
                        std::string::iterator versionBeginPtr = characterPtr + 1;
                    }
                    else if (!versionFound)
                    {

                    }

                }
            }            
        }
    }

    return true;
}
