
#ifndef LEVEN_NONCOPYABLE_H
#define LEVEN_NONCOPYABLE_H

namespace leven
{

class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

}

#endif //LEVEN_NONCOPYABLE_H
