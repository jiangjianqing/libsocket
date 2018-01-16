#ifndef ABSTRACTBUFFERPOOL_H
#define ABSTRACTBUFFERPOOL_H


class AbstractBufferPool
{
public:
    AbstractBufferPool();
    virtual ~AbstractBufferPool();

    virtual void* alloc(int suggested_size) = 0;
    virtual void dealloc(void* buf) = 0;

};

#endif // ABSTRACTBUFFERPOOL_H
