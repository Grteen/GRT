#ifndef GRT_CALLBACK_H
#define GRT_CALLBACK_H

#include <functional>
// define all callback function
namespace grt
{

class Buffer;

typedef std::function<void()> TimerCallback;

typedef std::function<void (Buffer& inputBuffer , Buffer& computBuf)> ReadFunction;
typedef std::function<void (Buffer& computBuf , Buffer& outputBuf)> ComputFunction;
typedef std::function<void (Buffer& outputBuf)> WriteFunction;

}
#endif