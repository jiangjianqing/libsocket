#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "AbstractSocket.h"

class UdpClient : public AbstractSocket
{
public:
    explicit UdpClient();
    virtual ~UdpClient();

    void close() override;
    bool connect(int port);
    void broadcast(const char* data,size_t len);

    void send(const uv_buf_t bufs[],unsigned int nbufs,const struct sockaddr* addr);
    void send(const char* data,size_t len,const struct sockaddr* addr);


private:
    int m_port;
    uv_udp_t m_sendHandle;
    SocketData* m_socketData;
    static void closeClientByThread(SocketData* cdata);
    static void closeClient(SocketData* cdata);
    static void onAfterSend(uv_udp_send_t* req, int status);
    static void onAfterRead(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void onAfterClientClose(uv_handle_t *handle);
};

#endif // UDPCLIENT_H
