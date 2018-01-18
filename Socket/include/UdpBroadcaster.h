#ifndef UDPBROADCASTER_H
#define UDPBROADCASTER_H

#include "AbstractSocket.h"

class UdpBroadcaster : public AbstractSocket
{
public:
    explicit UdpBroadcaster();
    virtual ~UdpBroadcaster();

    void close() override;
    bool start(int port);
    void broadcast(const char* data,size_t len);

    void send(const uv_buf_t bufs[],unsigned int nbufs,const struct sockaddr* addr);
    void send(const char* data,size_t len,const struct sockaddr* addr);


private:
    int m_port;
    SocketData* m_socketData;
    static void closeClientByThread(SocketData* cdata);
    static void closeClient(SocketData* cdata);
    static void onAfterSend(uv_udp_send_t* req, int status);
    static void onAfterRead(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
    static void onAfterClientClose(uv_handle_t *handle);
};

#endif // UDPBROADCASTER_H
