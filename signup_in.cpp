#include "./grt/Net/TcpServer.h"
#include "./grt/Http/HttpResponse.h"
#include "./grt/Http/HttpRequest.h"
#include "./grt/Log/Log.h"
#include <iostream>
#include <regex>
#include <sw/redis++/redis++.h>
#include <time.h>
#include <stdlib.h>
#include <memory>
#include <fstream>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;
using namespace grt;
using namespace sw::redis;

typedef std::unique_ptr<sql::Connection> SQLConnPtr;
typedef std::unique_ptr<sql::PreparedStatement> SQLPtstPtr;
typedef std::unique_ptr<sql::ResultSet> SQLResPtr;

Redis redis1("tcp://GrteenFL@127.0.0.1/2");

sql::Driver *driver;
SQLConnPtr con;
SQLPtstPtr pstmt;
SQLResPtr res;

void init() {
    driver = get_driver_instance();
    con.reset(driver->connect("tcp://127.0.0.1:3306/mydb", "root", "GrteenFL"));
    pstmt.reset(con->prepareStatement("create table if not exists account (id varchar(20) primary key , passwd varchar(20) not null)"));
    pstmt->executeUpdate();
}

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
        if (hq.RequestPath() == "/login") {
            std::string id = hq.GetURLByKey("id");
            std::string pw = hq.GetURLByKey("passwd");
            std::string vf = hq.GetURLByKey("verify");

            if (id != NOTFINDURLKEY && pw != NOTFINDURLKEY && vf == NOTFINDURLKEY) {
                pstmt.reset(con->prepareStatement("select id from account where id = ?"));
                pstmt->setString(1 , id);
                res.reset(pstmt->executeQuery());
                if (res->next() && res->getString(1) == id) {
                    hr.SetResponseBody("account have been registered");
                }
                else {
                    srand((unsigned)time(NULL));
                    long long res = rand() % 9000 + 1000;
                    std::pair<std::string , std::string> vfpair = make_pair("verify" , std::to_string(res));
                    std::pair<std::string , std::string> pwpair = make_pair("passwd" , pw);
                    redis1.hmset(id , {
                        vfpair ,
                        pwpair
                    });
                    redis1.expire(id , 60);
                    hr.SetResponseBody(std::to_string(res));
                }
            }
            else if (id != NOTFINDURLKEY && vf != NOTFINDURLKEY) {
                auto passwd = redis1.hget(id , "passwd");
                auto verify = redis1.hget(id , "verify");
                if (passwd && verify && verify == vf) {
                    redis1.del(id);
                    try {
                        pstmt.reset(con->prepareStatement("insert into account values(? , ?)"));
                        pstmt->setString(1 , id);
                        pstmt->setString(2 , *passwd);
                        pstmt->executeUpdate();
                        hr.SetResponseBody("log in ok");
                    } catch(sql::SQLException &e) {
                        cout << "# ERR: SQLException in " << __FILE__;
                        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
                        cout << "# ERR: " << e.what();
                        cout << " (MySQL error code: " << e.getErrorCode();
                        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
                        hr.SetResponseBody("something wrong");
                    }
                }
                else {
                    hr.SetResponseBody("verify not good");
                }
            }
        }
        else if (hq.RequestPath() == "/signin") {
            std::string id = hq.GetURLByKey("id");
            std::string pw = hq.GetURLByKey("passwd");
            pstmt.reset(con->prepareStatement("select id , passwd from account where id = ?"));
            pstmt->setString(1 , id);
            res.reset(pstmt->executeQuery());
            if (res->next() && res->getString(2) == pw) {
                hr.SetResponseBody("welcome");
            }
            else {
                hr.SetResponseBody("passwd is wrong");
            }
        }
        else if (hq.RequestPath() == "/fileUpload") {
            auto formdata = hq.RequestBodyInFormData();
            for (size_t i = 0 ; i < formdata.size() ; i++) {
                std::cout << formdata[i].GetValueByKey("name") << std::endl;
                ofstream of("./" + formdata[i].GetValueByKey("name"));
                of << formdata[i].ContentBody();
                of.close();
            }
        }
        else {
            hr.SetResponseBody("Hello World");
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
    
    init();
    server.setReadFunction(readFunction);
    server.setComputFunction(computFunction);
    server.setWriteFunction(writeFunction);
    server.setConnectionCallback(onConnection);

    server.setIOThreadNum(4);
    server.setComputThreadNum(4);

    server.start();
    loop.loop();
}