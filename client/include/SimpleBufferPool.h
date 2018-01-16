#ifndef SIMPLEBUFFERPOOL_H
#define SIMPLEBUFFERPOOL_H

#include "AbstractBufferPool.h"

class SimpleBufferPool : public AbstractBufferPool
{
public:
    SimpleBufferPool();

    void* alloc(int suggested_size) override;
    void dealloc(void* buf) override;
};

#endif // SIMPLEBUFFERPOOL_H
