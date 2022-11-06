#include "HttpResponse.h"

namespace grt
{

namespace http
{

HttpResponse::HttpResponse() {

}

HttpResponse::~HttpResponse() {

}

void HttpResponse::AddHeader(const std::string& key , const std::string value) {
    this->KeyValueMessage.insert(std::make_pair(key , value));
}

void HttpResponse::SetStatusCode(const std::string& statusCode , const std::string& status) {
    this->statusCode = statusCode;
    this->status = status;
}

void HttpResponse::SetResponseBody(const std::string& responseBody) {
    this->responseBody = responseBody;
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