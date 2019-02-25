#include <iostream>
#include <cassert>

#include "proxy_request_handler.h"

using boost::asio::buffers_begin;

ProxyRequestHandler::ProxyRequestHandler(
    std::unordered_map<std::string, std::string> config_params, std::string destPath):
    RequestHandler()
{
    dest = config_params["dest"];
    // std::cout << "destinated proxy address: " +  dest << std::endl;
    // uri = "";
    // filePath = "";
}

void ProxyRequestHandler::setURI(std::string request_uri)
{
    if(request_uri.length() > 1)
    {
        std::string::iterator it = request_uri.begin();
        if (*it == '/')
        {
            ++it;
            uriPath.push_back('/');
            for (; *it != '/' && it != request_uri.end(); ++it);
            if (it != request_uri.end())
            {
                ++it;
                for (; it != request_uri.end(); ++it)
                {
                    uriPath.push_back(*it);
                }
            }
        }
    }
    // std::cout << "uriPath: " +  uriPath << std::endl;
}

std::unique_ptr<RequestHandler> ProxyRequestHandler::create(const NginxConfig& config, const std::string& destPath){
    ProxyRequestHandler proxyHandler(config.getFlatParameters(), destPath);
    return std::make_unique<ProxyRequestHandler>(proxyHandler);
}

std::unique_ptr<HttpResponse> ProxyRequestHandler::HandleRequest(const HttpRequest &request){
    //
    std::cout << "Request URI before set: " << request.requestURI << "\n";
    setURI(request.requestURI);
    std::cout << "Request URI after set: " << uriPath << "\n";

    //test for sending request to dest server
    boost::asio::io_service io_service;

    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(dest, "80");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf req;
    std::ostream request_stream(&req);
    request_stream << "GET " << uriPath << " HTTP/1.0\r\n";
    request_stream << "Host: " << dest << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(socket, req);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      std::cout << "Invalid response\n";
    }
    if (status_code != 200)
    {
      std::cout << "Response returned with status code " << status_code << "\n";
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    std::map<std::string,std::string> headers;
    std::string header;
    while (std::getline(response_stream, header) && header != "\r"){
      std::string token = header.substr(0, header.find(":"));
      headers[token] = header.substr(header.find(":")+2);
      // std::cout << token << ":" << header.substr(header.find(":")+2) << std::endl;
      // std::cout << "header: " <<  header << "\n";
    }
    //
    // std::cout << "\n";

    std::string body;
    // Write whatever content we already have to output.
    if (response.size() > 0){
        //write the reponse content to string
        auto bufs = response.data();
        std::string content_string(buffers_begin(bufs), buffers_begin(bufs) + response.size());
        body = content_string;
    }

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while (boost::asio::read(socket, response,
           boost::asio::transfer_at_least(1), error)) {
        // std::cout << &response;
        auto bufs = response.data();
        std::string content_string(buffers_begin(bufs), buffers_begin(bufs) + response.size());
        body += content_string;
    }
    // std::cout << body;

    if (error != boost::asio::error::eof)
       //modify here to log later
       throw boost::system::system_error(error);

    status = status_code;
    std::string contentLengthStr = std::to_string(body.length());
    // std::map<std::string,std::string> headers {{"Content-Type", "text/html"},
    //                                             {"Content-Length", contentLengthStr}};
    HttpResponse res;
    res.setHttpResponse(status, headers, body);
    return std::make_unique<HttpResponse>(res);
}
