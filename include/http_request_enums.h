#ifndef HTTP_REQUEST_ENUMS_H
#define HTTP_REQUEST_ENUMS_H


namespace httpRequestEnum {

    enum httpMethod {
        GET = 0,
        HEAD = 1,
        POST = 2,
        PUT = 3,
        DELETE = 4,
        CONNECT = 5,
        OPTIONS = 6,
        TRACE = 7,
        PATCH = 8,
    };
}

#endif //HTTP_REQUEST_ENUMS_H
