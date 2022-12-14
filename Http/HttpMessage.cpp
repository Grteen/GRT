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

std::vector<std::string> URLSplit(std::string& URL , std::string& regexString) {
    std::string regexstring(regexString);
    return StringSplit(URL , regexstring);
}

size_t HttpFind(std::string& targetString) {
    std::regex reg("([.\\s\\S]+)\r\n\r\n");
    std::smatch sm;
    // if regex match the http message
    if (std::regex_search(targetString , sm , reg) == true) {
        return sm[0].length();
    }
    else {
        return 0;
    }
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

std::string HttpMessage::HasContentLength() {
    auto iter = this->KeyValueMessage.find("Content-Length");
    if (iter == this->KeyValueMessage.end()) {
        return "";
    }
    else {
        return iter->second;
    }
}

}

}