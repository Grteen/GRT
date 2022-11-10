#ifndef GRT_CALLBACK_H
#define GRT_CALLBACK_H

#include <functional>
#include <memory>
// define all callback function
namespace grt
{

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void()> TimerCallback;

typedef std::function<void (const TcpConnectionPtr& conn)> ReadFunction;
typedef std::function<void (const TcpConnectionPtr& conn)> ComputFunction;
typedef std::function<void (const TcpConnectionPtr& conn)> WriteFunction;

typedef std::function<void (const TcpConnectionPtr& conn)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr& conn)> CloseCallback;

}
#endif