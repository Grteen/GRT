#include "FormData.h"

namespace grt
{

namespace http
{

#ifndef NOTFINDURLKEY
#define NOTFINDURLKEY "NOTFINDURLKEY"
#endif

FormData::FormData() {}

FormData::~FormData() {}

void FormData::ParseFormDataAll(const std::string& requestBody) {
    this->ParseBody(requestBody);
    this->ParseKeyValueContentToPairs(this->ParseKeyValueContentToLines(requestBody));
}

void FormData::ParseBody(const std::string& requestBody) {
    size_t endindex = requestBody.find("\r\n\r\n");
    // set the contentBody (main source of a file)
    std::string contentBody(requestBody.begin() + endindex + 4 , requestBody.end() - 2);
    this->contentBody = contentBody;
}

std::vector<std::string> FormData::ParseKeyValueContentToLines(const std::string& requestBody) {
    size_t endindex = requestBody.find("\r\n");
    std::string keyValueBody(requestBody.begin() , requestBody.begin() + endindex);
    std::regex reg("; ");
    std::vector<std::string> res(std::sregex_token_iterator(keyValueBody.begin(), keyValueBody.end(), reg, -1) , std::sregex_token_iterator());
    return res;
}

void FormData::ParseKeyValueContentToPairs(const std::vector<std::string>& requestLines) {
    std::regex reg("([^\r\n]+)=([^\r\n]+)");
    std::smatch sm;
    for (size_t i = 0 ; i < requestLines.size() ; i++) {
        if (i == 0) {
            // Content-Disposition
            std::regex regSp("([^\r\n]+): ([^\r\n]+)");
            std::regex_search(requestLines[0] , sm , regSp);
            this->keyValueContent.insert(std::make_pair(sm[1] , sm[2]));
        }
        else {
            std::regex_search(requestLines[i] , sm , reg);
            this->keyValueContent.insert(std::make_pair(sm[1] , sm[2]));
        }
    }
}

std::string FormData::GetValueByKey(const std::string& key) {
    auto iter = this->keyValueContent.find(key);
    if (iter != this->keyValueContent.end()) {
        return iter->second;
    }
    else {
        return NOTFINDURLKEY;
    }
}

}

}