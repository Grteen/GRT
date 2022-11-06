#include "HttpMessage.h"

namespace grt
{

namespace http
{

class HttpRequest : public HttpMessage {
public:
    HttpRequest();
    ~HttpRequest();

    // parse the whole request message
    void ParseAll(std::string& HttpRequest);
private:
    void ParseURL(std::string& URL);
    void ParseURLByLines(std::vector<std::string>& URLlines);
    void ParseRequestWay(std::string& requestLine);
    void ParseHttpVersion(std::string& requestLine);
private:
    // request line
    // GET or POST
    std::string requestWay;
    // URL
    std::string requestURL;
    // parseURL
    std::unordered_map<std::string , std::string> keyValueURL;
    // Http Version
    std::string httpVersion;
};

}

}