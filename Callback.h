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

typedef std::function<void (Buffer& inputBuffer , Buffer& computBuf)> ReadFunction;
typedef std::function<void (Buffer& computBuf , Buffer& outputBuf)> ComputFunction;
typedef std::function<void (Buffer& outputBuf)> WriteFunction;

typedef std::function<void (const TcpConnectionPtr& conn)> ConnectionCallback;

}
#endif