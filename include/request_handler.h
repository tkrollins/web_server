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
    virtual std::vector<const_buffer> getResponseBuffer() = 0;
    RequestHandler() {}
    virtual bool canHandleRequest(HttpRequest req) = 0;
private:
    std::vector<const_buffer> responseBuffer;
};


#endif //REQUEST_HANDLER_H
