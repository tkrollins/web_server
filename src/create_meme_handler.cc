# include "create_meme_handler.h"
# include "http_response.h"
# include "meme_db.h"
# include <algorithm>
# include <boost/uuid/uuid.hpp>
# include <boost/uuid/uuid_generators.hpp>
# include <boost/uuid/uuid_io.hpp>
# include <chrono>

namespace TIME = std::chrono;

std::unique_ptr<RequestHandler> CreateMemeHandler::create(const NginxConfig& config, const std::string& root_path)
{
    CreateMemeHandler createHandler;
    return std::make_unique<CreateMemeHandler>(createHandler);
}

std::string CreateMemeHandler::escape(const std::string& data) {
    std::string res;
    res.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  res.append("&amp;");       break;
            case '\"': res.append("&quot;");      break;
            case '\'': res.append("&#39;");       break;
            case '<':  res.append("&lt;");        break;
            case '>':  res.append("&gt;");        break;
            default:   res.append(data, pos, 1);  break;
        }
    }
    return res;
}

//https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
std::string CreateMemeHandler::urlDecode(const std::string &SRC)
{
    std::string ret;
    char ch;
    int i, ii;
    for (i=0; i<SRC.length(); i++) {
        if (int(SRC[i])==37) {
            sscanf(SRC.substr(i+1,2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            ret+=ch;
            i=i+2;
        } else {
            ret+=SRC[i];
        }
    }
    return (ret);
}

bool CreateMemeHandler::parseRequestBody(const HttpRequest &request)
{
    if(request.requestMethod != httpRequestEnum::POST)
    {
        return false;
    }
    int andSignPos = 0;
    std::string urlParams = urlDecode(request.requestBody);
    std::replace(urlParams.begin(), urlParams.end(), '+', ' ');
    urlParams += "&"; // in order to successfully parse the last parameter
    andSignPos = urlParams.find("&");
    while(andSignPos != std::string::npos)
    {
        std::string token = urlParams.substr(0, andSignPos);
        int equalSignPos = token.find("=");
        if(equalSignPos != std::string::npos)
        {
            std::string key = token.substr(0, equalSignPos);
            std::string value = token.substr(equalSignPos+1);
            if(value.length() >= 1)
            {
                // prevent malicious input
                params[key] = escape(value);
            }
        }
        urlParams.erase(0, andSignPos+1);
        andSignPos = urlParams.find("&");
    }
    if(params.find("image") != params.end() && params["image"] == "Select a template...")
    {
        return false;
    }
    return params.find("image") != params.end()  && params.find("top") != params.end() && params.find("bottom") != params.end();
}

std::unique_ptr<HttpResponse> CreateMemeHandler::HandleRequest(const HttpRequest &request)
{
    // parse the request path
    bool successfullyParsed = parseRequestBody(request);
    if(successfullyParsed)
    {
        std::string timestamp = std::to_string(TIME::duration_cast<TIME::milliseconds>(TIME::system_clock::now().time_since_epoch()).count());

        std::string id;
        // if we are updating an existing meme, the post request body will contain an update param with an 
        // existing meme's ID. Set the database key value to this id
        if (params.count("update")) 
        {
            id = params["update"];
        }
        else
        {
            // randomly generate 128 bit, the probability of duplication can be neglected
            id = boost::uuids::to_string(boost::uuids::random_generator()());
        }

        // save the stuff into database
        MemeDB database;
        database.Put(id, params["image"], MemeDB::IMAGE);
        database.Put(id, params["top"], MemeDB::TOP_TEXT);
        database.Put(id, params["bottom"], MemeDB::BOTTOM_TEXT);
        database.Put(id, timestamp, MemeDB::TIMESTAMP);

        // return value
        std::string body = "Created meme! <a href=\"/meme/view?id=" + id + "\">" + id + "</a>";
        std::map<std::string,std::string> headers { {"Content-Type", "text/html"},
                                                    {"Content-Length", std::to_string(body.length())} };
        HttpResponse res;
        res.setHttpResponse(200, headers, body);
        std::unique_ptr<HttpResponse> resPtr = std::make_unique<HttpResponse>(res);
        return resPtr;
    }
    else
    {
        std::string body = "404 not found\r\n";
        std::map<std::string,std::string> headers { {"Content-Type", "text/plain"},
                                                    {"Content-Length", std::to_string(body.length())} };
        HttpResponse res;
        res.setHttpResponse(404, headers, body);
        std::unique_ptr<HttpResponse> resPtr = std::make_unique<HttpResponse>(res);
        return resPtr;
    }

}