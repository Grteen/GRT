#ifndef GRT_BASE_NONCOPYABLE_H
#define GRT_BASE_NONCOPYABLE_H
namespace grt
{

namespace base
{

class noncopyable {
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

}

}
#endif