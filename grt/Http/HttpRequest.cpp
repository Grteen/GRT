#include "HttpRequest.h"

namespace grt
{

namespace http
{

size_t HttpHave(std::string& targetString) {
    size_t headerLength = http::HttpFind(targetString);
    if (headerLength == 0) {
        return 0;
    }
    
    std::string httpHeaders(targetString.begin() , targetString.begin() + headerLength);
    http::HttpRequest hq;
    hq.ParseHttpMessage(httpHeaders);
    std::string bodyLength = hq.HasContentLength();
    // do not have the header Content-Length
    if (bodyLength == "") {
        return headerLength;
    }
    else {
        return headerLength + stoi(bodyLength);
    }
}

HttpRequest::HttpRequest() {

}

HttpRequest::~HttpRequest() {

}

void HttpRequest::ParseURLByLines(std::vector<std::string>& URL) {
    std::regex reg("([^? ]+)=([^ \r\n]+)");
    std::smatch sm;
    for (auto line : URL) {
        regex_search(line , sm , reg);
        this->keyValueURL.insert(std::make_pair(sm[1] , sm[2]));
    }
}

void HttpRequest::ParseURL(std::string& URL) {
    std::regex reg("[^ ]+ ([^ ]+)");
    std::smatch sm;
    regex_search(URL , sm , reg);
    this->requestURL = sm[1];
    std::string regexString = "&";
    std::vector<std::string> URLlines = URLSplit(this->requestURL , regexString);
    this->ParseURLByLines(URLlines);

    std::regex rg("[^ ]+ ([^? ]+)");
    regex_search(URL , sm , rg);
    this->requestPath = sm[1];
}

void HttpRequest::ParseRequestWay(std::string& requestLine) {
    // search the first word
    std::regex reg("[^ \r\n]+");
    std::smatch sm;
    regex_search(requestLine , sm , reg);
    this->requestWay = sm[0];
}

void HttpRequest::ParseHttpVersion(std::string& requestLine) {
    // search the final word
    std::regex reg("[^ ]+ [^ ]+ ([^ \r\n]+)");
    std::smatch sm;
    regex_search(requestLine , sm , reg);
    this->httpVersion = sm[1];
}

void HttpRequest::ParseRequestBody(std::string& requestLine) {
    std::string boundary = this->GetHeaderByKey("boundary");
    if (boundary != "") {
        size_t index = requestLine.find("--" + boundary);
        index += 3;
        index += boundary.size();
        size_t endindex = requestLine.rfind(boundary);
        endindex -= 5;
        std::string temp(requestLine.begin() + index , requestLine.begin() + endindex);
        size_t startindex = temp.find("\r\n\r\n") + 4;
        this->requestBody = std::string(temp.begin() + startindex , temp.end());
    }
    else {
        size_t index = requestLine.find("\r\n\r\n");
        index += 4;
        this->requestBody = std::string(requestLine.begin() + index , requestLine.end());
    }
}

void HttpRequest::ParseAll(std::string& HttpRequest) {
    this->ParseHttpMessage(HttpRequest);
    this->ParseURL(HttpRequest);
    this->ParseRequestWay(HttpRequest);
    this->ParseHttpVersion(HttpRequest);
    if (this->requestWay == "POST") {
        this->ParseBoundary(this->GetHeaderByKey("Content-Type"));
        this->ParseRequestBody(HttpRequest);
    }
}

std::string HttpRequest::GetURLByKey(const std::string& first) {
    auto res_it = this->keyValueURL.find(first);
    if (res_it != this->keyValueURL.end()) {
        return res_it->second;
    }
    else {
        return NOTFINDURLKEY;
    }
}

}

}