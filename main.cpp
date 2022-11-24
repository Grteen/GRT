#include "./grt/Net/TcpServer.h"
#include "./grt/Http/HttpResponse.h"
#include "./grt/Http/HttpRequest.h"
#include "./grt/Log/Log.h"
#include <iostream>
#include <regex>

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
        if (hq.RequestURL() == "/FAV") {
            hr.SetResponseBody("NB");
        }
        else if (hq.RequestURL() == "/favicon.ico") {
            hr.SetResponseBody("TEMP");
        }
        else {
            hr.SetResponseBody("Hello World");
        }
        
        std::string id = hq.GetURLByKey("id");
        std::string name = hq.GetURLByKey("name");
        if (id != NOTFINDURLKEY) {
            hr.AddResponseBody("\n" + id);
        }
        if (name != NOTFINDURLKEY) {
            hr.AddResponseBody("\n" + name);
        }

        if (hq.RequestWay() == "POST") {
            hr.AddResponseBody(hq.RequestBody());
        }
        
        hr.SetStatusCode("200" , "OK");
        hr.AddHeader("Content-Type" , "text/plain;charset=utf-8");
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