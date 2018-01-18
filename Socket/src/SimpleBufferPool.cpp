#include "SimpleBufferPool.h"
#include <stdlib.h>

SimpleBufferPool::SimpleBufferPool()
{

}

void* SimpleBufferPool::alloc(int suggested_size)
{
    return malloc(suggested_size);
}

void SimpleBufferPool::dealloc(void* buf)
{
    free(buf);
}
