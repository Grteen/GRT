#include <regex>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

namespace grt
{

namespace http
{

std::vector<std::string> URLSplit(std::string& URL);
std::vector<std::string> HttpSplit(std::string& httpMessage);
std::vector<std::string> StringSplit(std::string& targetString , std::string& regexString);

class HttpMessage {
public:
    HttpMessage();
    virtual ~HttpMessage();
protected:
    void ParseHttpMessage(std::string& httpMessage);
    void ParseHttpMessageByLines(std::vector<std::string>& rawMessageLines);
protected:
    // response or request's headers
    std::unordered_map<std::string , std::string> KeyValueMessage;
};

}

}