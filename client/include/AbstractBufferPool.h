#ifndef ABSTRACTBUFFERPOOL_H
#define ABSTRACTBUFFERPOOL_H


class AbstractBufferPool
{
public:
    AbstractBufferPool();
    virtual ~AbstractBufferPool();

    virtual char* alloc(size_t suggested_size) = 0;
    virtual void dealloc(const char* buf) = 0;

};

#endif // ABSTRACTBUFFERPOOL_H
