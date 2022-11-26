#include "HttpMessage.h"
#include "FormData.h"

namespace grt
{

namespace http
{

#define NOTFINDURLKEY "NOTFINDURLKEY"

// used to find the complete http message
size_t HttpHave(std::string& targetString);

class HttpRequest : public HttpMessage {
public:
    HttpRequest();
    ~HttpRequest();

    // parse the whole request message
    void ParseAll(std::string& HttpRequest);

    std::string HttpVersion() { return this->httpVersion; }
    std::string RequestWay() { return this->requestWay; }
    std::string RequestURL() { return this->requestURL; }
    std::string RequestPath() { return this->requestPath; }
    std::string RequestBody() { return this->requestBody; }
    std::unordered_map<std::string , std::string> KeyValueURL() { return this->keyValueURL; }
    std::string GetURLByKey(const std::string& first);
    std::vector<FormData> RequestBodyInFormData() { return this->requestBodyInFormData; }
private:
    void ParseURL(std::string& URL);
    void ParseURLByLines(std::vector<std::string>& URLlines);
    void ParseRequestWay(std::string& requestLine);
    void ParseHttpVersion(std::string& requestLine);
    void ParseRequestBody(std::string& requestLine);
    void ParseRequestBodyInApplication(const std::string& requestString);
    void ParseRequestBodyInFormData(const std::string& requestString);
private:
    // request line
    // GET or POST
    std::string requestWay;
    // URL
    std::string requestURL;
    // URL without parmeter
    std::string requestPath;
    // parseURL
    std::unordered_map<std::string , std::string> keyValueURL;
    // Http Version
    std::string httpVersion;
    // POST's body
    std::string requestBody;
    // POST's formdata
    std::vector<FormData> requestBodyInFormData;
};

}

}