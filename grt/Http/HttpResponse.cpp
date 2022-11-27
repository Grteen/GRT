#include "HttpResponse.h"
#include <string>
#include <fstream>

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

void HttpResponse::SetResponseBody(const std::string& responseBody , const FILETYPE& type) {
    if (type == text) {
        this->SetResponseBodyInText(responseBody);
    }
    else if (type == image) {
        this->SetResponseBodyInImage(responseBody);
    }
    size_t bodyLength = this->responseBody.length();
    std::string length = std::to_string(static_cast<int>(bodyLength));
    this->AddHeader("Content-Length" , length);
}

void HttpResponse::SetResponseBody(const std::string& responseBody) {
    this->SetResponseBody(responseBody , text);
}

void HttpResponse::SetResponseBodyInText(const std::string& responseBody) {
    this->responseBody = responseBody;
}

void HttpResponse::SetResponseBodyInImage(const std::string& imagePath) {
    std::ifstream is(imagePath , std::ios::in | std::ios::binary);
    std::stringstream buffer; 
    buffer << is.rdbuf();
    std::string res(buffer.str());
    is.close();
    this->responseBody = res;

    std::string suffix;
    std::smatch sm;
    std::regex reg(".+\\.(.+)");
    std::regex_search(imagePath , sm , reg);
    this->AddHeader("Content-Type" , "image/" + std::string(sm[1]));
}

void HttpResponse::AddResponseBody(const std::string& responseBody) {
    this->responseBody += responseBody;
    size_t bodyLength = this->responseBody.length();
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