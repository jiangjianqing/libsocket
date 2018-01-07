#include "SocketData.h"
#include <stdlib.h>

#define BUFFERSIZE (1024*1024)

SocketData::SocketData(int clientId,AbstractSocket* socket)
    : m_clientId(clientId),m_socket(socket),recvcb_(nullptr)
{
    //client_handle = (uv_tcp_t*)malloc(sizeof(*client_handle));
    m_socketHandle = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    m_socketHandle->data = this;
    readbuffer = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
    writebuffer = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
}

SocketData::~SocketData()
{
    free(readbuffer.base);
    readbuffer.base = nullptr;
    readbuffer.len = 0;

    free(writebuffer.base);
    writebuffer.base = nullptr;
    writebuffer.len = 0;

    free(m_socketHandle);
    m_socketHandle = nullptr;
}
