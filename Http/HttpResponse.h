#include "HttpMessage.h"

namespace grt
{

namespace http
{

class HttpResponse : public HttpMessage {
public:
    HttpResponse();
    ~HttpResponse();

    void AddHeader(const std::string& key , const std::string& value);
    void SetStatusCode(const std::string& statusCode , const std::string& status);
    void SetResponseBody(const std::string& responseBody);
    void setHttpVersion(const std::string& httpVersion);

    // generate Http response as string
    std::string GenerateResponseString();
private:
private:
    // Http Version
    std::string httpVersion;

    // statuscode and it's description
    std::string status;
    std::string statusCode;

    // responseBody
    std::string responseBody;
};

}

}