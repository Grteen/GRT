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
    // std::regex reg("([\\S\\s]+)\r\n\r\n");
    // std::smatch sm;
    // // if regex match the http message
    // if (std::regex_search(targetString , sm , reg) == true) {
    //     std::cout << sm[0] << std::endl;
    //     return sm[0].length();
    // }
    // else {
    //     return 0;
    // }

    size_t index = targetString.find("\r\n\r\n");
    if (index < targetString.size()) {
        return index + 4;
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
    std::regex reg("([^\r\n]+): ([^\r\n]+)");
    std::smatch sm;
    for (auto line : rawMessageLines) {
        regex_search(line , sm , reg);
        this->KeyValueMessage.insert(std::make_pair(sm[1] , sm[2]));
    }
}

void HttpMessage::ParseBoundary(const std::string& type) {
    std::regex reg("[^\r\n]+; ([^\r\n]+)=([^\r\n]+)");
    std::smatch sm;
    std::regex_search(type , sm , reg);
    this->KeyValueMessage.insert(std::make_pair(sm[1] , sm[2]));
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
    else if (iter->second == "0") {
        return "";
    }
    else {
        return iter->second;
    }
}

std::string HttpMessage::GetHeaderByKey(const std::string& first) {
    auto iter = this->KeyValueMessage.find(first);
    if (iter == this->KeyValueMessage.end()) {
        return "";
    }
    else {
        return iter->second;
    }
}

}

}