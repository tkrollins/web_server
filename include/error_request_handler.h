#ifndef ERROR_REQUEST_HANDLER_H
#define ERROR_REQUEST_HANDLER_H

#include "request_handler.h"


class ErrorRequestHandler : public RequestHandler
{
public:
    ErrorRequestHandler() { status = 400; }
    bool canHandleRequest(HttpRequest req) override;
    void handleRequest(std::string* response) override;
};


#endif //ERROR_REQUEST_HANDLER_H
