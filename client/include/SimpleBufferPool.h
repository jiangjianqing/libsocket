#ifndef SIMPLEBUFFERPOOL_H
#define SIMPLEBUFFERPOOL_H

#include "AbstractBufferPool.h"

class SimpleBufferPool : public AbstractBufferPool
{
public:
    SimpleBufferPool();

    char* alloc(size_t suggested_size) override;
    void dealloc(const char* buf) override;
};

#endif // SIMPLEBUFFERPOOL_H
