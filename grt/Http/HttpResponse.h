#include "HttpMessage.h"
#include <regex>

namespace grt
{

namespace http
{

enum FILETYPE {
    text ,
    image ,
};

class HttpResponse : public HttpMessage {
public:
    HttpResponse();
    ~HttpResponse();

    void AddHeader(const std::string& key , const std::string& value);
    void SetStatusCode(const std::string& statusCode , const std::string& status);
    void SetResponseBody(const std::string& responseBody , const FILETYPE& type);
    // default is set the text type
    void SetResponseBody(const std::string& responseBody);
    void AddResponseBody(const std::string& responseBody);
    void setHttpVersion(const std::string& httpVersion);

    // generate Http response as string
    std::string GenerateResponseString();
private:
    void SetResponseBodyInText(const std::string& responseBody);
    void SetResponseBodyInImage(const std::string& imagePath);
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