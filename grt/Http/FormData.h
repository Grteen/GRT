#ifndef GRT_HTTP_FORMDATA_H
#define GRT_HTTP_FORMDATA_H

#include <unordered_map>
#include <iostream>
#include <string>
#include <regex>

namespace grt
{

namespace http
{

class FormData {
public:
    FormData();
    ~FormData();
    void ParseFormDataAll(const std::string& requestBody);
    std::string ContentBody() { return this->contentBody; }
    std::string GetValueByKey(const std::string& key);
private:
    void ParseBody(const std::string& requestBody);
    std::vector<std::string> ParseKeyValueContentToLines(const std::string& requestBody);
    void ParseKeyValueContentToPairs(const std::vector<std::string>& requestLines);
private:
    std::unordered_map<std::string , std::string> keyValueContent;
    std::string contentBody;
};

}

}

#endif