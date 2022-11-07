#include "TcpServer.h"
#include "./Http/HttpResponse.h"
#include "./Http/HttpRequest.h"
#include "Log/Log.h"
#include <iostream>
#include <regex>

using namespace std;
using namespace grt;

void readFunction(Buffer& inputBuf , Buffer& computBuf) {
    computBuf.append(inputBuf.retrieveAllAsString());
}

void computFunction(Buffer& computBuf , Buffer& outputBuf) {
    string str(computBuf.peek());
    size_t length = http::HttpHave(str);
    if (length == 0) {
        return;
    }
    while (length <= computBuf.readableBytes()) {
        string http = computBuf.retrieveAsString(length);
        // cout << http << endl;
        length = http::HttpHave(str);
        http::HttpRequest hq;
        http::HttpResponse hr;
        hq.ParseAll(http);
        hr.setHttpVersion(hq.HttpVersion());
        if (hq.RequestURL() == "/FAV") {
            hr.SetResponseBody("NB");
            hr.AddHeader("Content-Length" , "1321");
            hr.AddHeader("Content-Length" , "2");
        }
        else if (hq.RequestURL() == "favicon.ico") {
            hr.SetResponseBody("TEMP");
            hr.AddHeader("Content-Length" , "4");
        }
        else {
            hr.SetResponseBody("Hello World");
            hr.AddHeader("Content-Length" , "11");
        }
        hr.SetStatusCode("200" , "OK");
        hr.AddHeader("Content-Type" , "text/plain;charset=utf-8");
        outputBuf.append(hr.GenerateResponseString());
        // LOG(INFO , "%s" , "message send ok");
    }
}

void writeFunction(Buffer& outputBuf) {

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

    server.setIOThreadNum(8);
    server.setComputThreadNum(0);

    server.start();
    loop.loop();
}