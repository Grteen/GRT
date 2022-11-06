#include "HttpMessage.h"

namespace grt 
{

namespace http
{

std::vector<std::string> StringSplit(std::string& targetString , std::string& regexString) {
    std::regex reg(regexString);
    std::vector<std::string> res(std::sregex_token_iterator(targetString.begin(), targetString.end(), reg, -1) , std::sregex_token_iterator());
    return res;
}

std::vector<std::string> HttpSplit(std::string& httpMessage) {
    std::string regexString("\r\n");
    return StringSplit(httpMessage , regexString);
}

std::vector<std::string> URLSplit(std::string& URL) {
    std::string regexString("&");
    return StringSplit(URL , regexString);
}

HttpMessage::HttpMessage() {

}

HttpMessage::~HttpMessage() {

}

void HttpMessage::ParseHttpMessageByLines(std::vector<std::string>& rawMessageLines) {
    std::regex reg("([0-9A-z]+): ([^ \r\n]+)");
    std::smatch sm;
    for (auto line : rawMessageLines) {
        regex_search(line , sm , reg);
        this->KeyValueMessage.insert(std::make_pair(sm[1] , sm[2]));
    }
}

void HttpMessage::ParseHttpMessage(std::string& httpMessage) {
    std::vector<std::string> httpMessageLines = HttpSplit(httpMessage);
    this->ParseHttpMessageByLines(httpMessageLines);
}

}

}