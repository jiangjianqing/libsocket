#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "AbstractSocket.h"
#include "SocketData.h"

class TcpClient : public AbstractSocket
{
public:
    explicit TcpClient();
    virtual ~TcpClient();

    //bool connect(const string &ip, int port) override;
    void close() override;
    bool connect(const char* ip, int port);

    bool send(const char* data, std::size_t len);


protected:

    void ConnectThread(const char* ip, int port);

    //静态回调函数
    static void closeCient(SocketData* cdata);

    static void onAfterConnect(uv_connect_t* req, int status);

    static void onAfterRead(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf);
    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

    static void onAfterClientClose(uv_handle_t *handle);
    static void onAfterWrite(uv_write_t *req, int status);

private:

     SocketData* m_socketData;

     //handle需要使用m_socketData的，从AbstractSocket继承来的m_socket忽略
     //uv_tcp_t* handle(){return m_socket;}
};

#endif // TCPCLIENT_H
