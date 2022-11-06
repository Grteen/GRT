#include "TcpServer.h"
#include "./Http/HttpResponse.h"
#include "Log/Log.h"
#include <iostream>

using namespace std;
using namespace grt;

void readFunction(Buffer& inputBuf , Buffer& computBuf) {
    computBuf.append(inputBuf.retrieveAllAsString());
}

void computFunction(Buffer& computBuf , Buffer& outputBuf) {
    http::HttpResponse hr;
    hr.setHttpVersion("HTTP/1.1");
    hr.SetResponseBody("Hello World");
    hr.SetStatusCode("200" , "OK");
    hr.AddHeader("Connection" , "keep-alive");
    hr.AddHeader("Content-Length" , "11");
    hr.AddHeader("Content-Type" , "text/plain;charset=utf-8");
    outputBuf.append(hr.GenerateResponseString());
    LOG(INFO , "message send ok");
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