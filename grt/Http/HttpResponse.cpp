#include "HttpResponse.h"
#include <string>

namespace grt
{

namespace http
{

HttpResponse::HttpResponse() {

}

HttpResponse::~HttpResponse() {

}

void HttpResponse::AddHeader(const std::string& key , const std::string& value) {
    if (this->KeyValueMessage.find(key) != this->KeyValueMessage.end()) 
        this->KeyValueMessage.erase(key);
    this->KeyValueMessage.insert(std::make_pair(key , value));
}

void HttpResponse::SetStatusCode(const std::string& statusCode , const std::string& status) {
    this->statusCode = statusCode;
    this->status = status;
}

void HttpResponse::SetResponseBody(const std::string& responseBody) {
    this->responseBody = responseBody;
    size_t bodyLength = responseBody.length();
    std::string length = std::to_string(static_cast<int>(bodyLength));
    this->AddHeader("Content-Length" , length);
}

void HttpResponse::setHttpVersion(const std::string& httpVersion) {
    this->httpVersion = httpVersion;
}

std::string HttpResponse::GenerateResponseString() {
    std::string httpResponse = "";
    httpResponse = httpResponse + this->httpVersion + " " + this->statusCode + " " + this->status + "\r\n";
    for (auto header : this->KeyValueMessage) {
        httpResponse = httpResponse + header.first + ": " + header.second + "\r\n";
    }
    httpResponse = httpResponse + "\r\n";
    httpResponse = httpResponse + this->responseBody + "\r\n";
    return httpResponse;
}

}

}