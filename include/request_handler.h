#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <vector>
#include "http_request.h"
#include "config_parser.h"

using boost::asio::const_buffer;

class RequestHandler
{
public:
	//The following function must be implemented in all handler subclass
    // static RequestHandler* create(const NginxConfig& config, const std::string& root_path);
    virtual void handleRequest(std::string* response) = 0;
    RequestHandler() = default;
    virtual bool canHandleRequest(HttpRequest req) = 0;
    virtual ~RequestHandler() {}

protected:
    int status;
};


#endif // REQUEST_HANDLER_H
