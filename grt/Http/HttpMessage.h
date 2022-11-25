#ifndef GRT_HTTP_HTTPMESSAGE_H
#define GRT_HTTP_HTTPMESSAGE_H
#include <regex>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

namespace grt
{

namespace http
{

std::vector<std::string> URLSplit(std::string& URL , std::string& regexString);
std::vector<std::string> HttpSplit(std::string& httpMessage);
std::vector<std::string> StringSplit(std::string& targetString , std::string& regexString);
// used to find the Http message
size_t HttpFind(std::string& targetString);

class HttpMessage {
public:
    HttpMessage();
    virtual ~HttpMessage();

    std::string HasContentLength();
    std::string GetHeaderByKey(const std::string& first);
    void ParseHttpMessage(std::string& httpMessage);
protected:
    void ParseHttpMessageByLines(std::vector<std::string>& rawMessageLines);
    void ParseBoundary(const std::string& type);
protected:
    // response or request's headers
    std::unordered_map<std::string , std::string> KeyValueMessage;
};

}

}

#endif