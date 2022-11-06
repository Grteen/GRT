#include "HttpRequest.h"

namespace grt
{

namespace http
{

HttpRequest::HttpRequest() {

}

HttpRequest::~HttpRequest() {

}

void HttpRequest::ParseURLByLines(std::vector<std::string>& URL) {
    std::regex reg("([0-9A-z]+)=([^ \r\n]+)");
    std::smatch sm;
    for (auto line : URL) {
        regex_search(line , sm , reg);
        this->keyValueURL.insert(std::make_pair(sm[1] , sm[2]));
    }
}

void HttpRequest::ParseURL(std::string& URL) {
    std::vector<std::string> URLlines = URLSplit(URL);
    this->ParseURLByLines(URLlines);
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

void HttpRequest::ParseAll(std::string& HttpRequest) {
    this->ParseHttpMessage(HttpRequest);
    this->ParseURL(HttpRequest);
    this->ParseRequestWay(HttpRequest);
    this->ParseHttpVersion(HttpRequest);
}

}

}