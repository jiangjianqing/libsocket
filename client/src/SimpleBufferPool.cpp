#include "SimpleBufferPool.h"
#include <stdlib.h>

SimpleBufferPool::SimpleBufferPool()
{

}

char* SimpleBufferPool::alloc(size_t suggested_size) override
{
    return (char*)malloc(suggested_size);
}

void SimpleBufferPool::dealloc(const char* buf) override
{
    free(buf);
}
