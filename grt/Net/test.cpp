#include "TcpServer.h"
#include "./Http/HttpResponse.h"
#include "./Http/HttpRequest.h"
#include "Log/Log.h"
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
        // cout << http << endl;
        length = http::HttpHave(str);
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
        hr.SetStatusCode("200" , "OK");
        hr.AddHeader("Content-Type" , "text/plain;charset=utf-8");
        conn->outputBuffer()->append(hr.GenerateResponseString());
    }
}

void writeFunction(const TcpConnectionPtr& conn) {
    LOG(INFO , "%s" , "message send ok");
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