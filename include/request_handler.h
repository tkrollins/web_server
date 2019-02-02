#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <vector>
#include "http_request.h"


using boost::asio::const_buffer;

class RequestHandler
{
public:
    // The inputs of this are a little weird to make static handler work
    // TODO: refactor this function to have more straightforward API
    virtual void handleRequest(std::string* header, std::vector<char>* file) = 0;
    RequestHandler() = default;
    virtual bool canHandleRequest(HttpRequest req) = 0;

protected:
    std::vector<const_buffer>* responseBuffer;
    std::string status;
};


#endif // REQUEST_HANDLER_H
