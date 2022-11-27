#include "./grt/Net/TcpServer.h"
#include "./grt/Http/HttpResponse.h"
#include "./grt/Http/HttpRequest.h"
#include "./grt/Log/Log.h"
#include <iostream>
#include <regex>
#include <time.h>
#include <stdlib.h>
#include <memory>
#include <fstream>

using namespace std;
using namespace grt;

void readFunction(const TcpConnectionPtr& conn) {

}

void computFunction(const TcpConnectionPtr& conn) {
    string str = (conn->inputBuffer()->peek());
    size_t length = http::HttpHave(str);
    if (length == 0) {
        return;
    }
    while (length <= conn->inputBuffer()->readableBytes()) {
        string http = conn->inputBuffer()->retrieveAsString(length);
        http::HttpRequest hq;
        http::HttpResponse hr;
        hq.ParseAll(http);
        hr.setHttpVersion(hq.HttpVersion());
        if (hq.RequestPath() == "/fileUpload" && hq.RequestWay() == "POST") {
            auto formdata = hq.RequestBodyInFormData();
            for (size_t i = 0 ; i < formdata.size() ; i++) {
                std::cout << formdata[i].GetValueByKey("name") << std::endl;
                ofstream of("./" + formdata[i].GetValueByKey("name"));
                of << formdata[i].ContentBody();
                of.close();
            }
        }
        else if (hq.RequestPath() == "/picture" && hq.RequestWay() == "GET") {
            hr.SetResponseBody("bm.jpg" , http::FILETYPE::image);
        }
        else if (hq.RequestPath() == "/test" && hq.RequestWay() == "GET") {
            std::string id = hq.GetURLByKey("id");
            std::string pw = hq.GetURLByKey("pw");
            hr.SetResponseBody("very good");
            hr.AddResponseBody("id = " + id + "pw = " + pw);
        }
        else if (hq.RequestPath() == "/test" && hq.RequestWay() == "POST") {
            hr.SetResponseBody(hq.RequestBody());
        }
        else {
            hr.SetResponseBody("Hello World");
        }
        hr.SetStatusCode("200" , "OK");
        hr.AddHeader("Access-Control-Allow-Origin", "*");
        conn->outputBuffer()->append(hr.GenerateResponseString());
        LOG(INFO , "%s" , "message send ok");
    }
}

void writeFunction(const TcpConnectionPtr& conn) {
    if (conn->outputBuffer()->readableBytes()) {
        conn->send();
    }
}

void onConnection(const TcpConnectionPtr& conn) {
    LOG(INFO , "NEW CONNECTION");
}

int main(void) {
    EventLoop loop;
    InetAddr listenAddr(9877);
    TcpServer server(&loop , listenAddr);
    
    server.setReadFunction(readFunction);
    server.setComputFunction(computFunction);
    server.setWriteFunction(writeFunction);
    server.setConnectionCallback(onConnection);

    server.setIOThreadNum(4);
    server.setComputThreadNum(4);

    server.start();
    loop.loop();
}