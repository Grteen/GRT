#ifndef GRT_COMPUTOR_H
#define GRT_COMPUTOR_H

#include <memory>
#include <functional>

namespace grt
{

class Buffer;

class Computor {
public:
    typedef std::function<void (Buffer& inputBuf , Buffer& computOverBuf)> computFunction;

    Computor(computFunction& cf , Buffer& inputBuf , Buffer& computOverBuf)
        : func_(cf) ,
          inputBuffer_(&inputBuf) ,
          computOverBuffer_(&computOverBuf)
    {}
    
    void callFunction() {
        this->func_(*(this->inputBuffer_.get()) , *(this->computOverBuffer_.get()));
    }
private:

private:
    computFunction func_;
    std::shared_ptr<Buffer> inputBuffer_;
    std::shared_ptr<Buffer> computOverBuffer_;
};

}

#endif