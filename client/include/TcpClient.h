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

    string remoteIp(){return m_remoteIp;}
    int remotePort(){return m_remotePort;}


protected:

    void refreshInfo() override;

    void ConnectThread(const char* ip, int port);

    //静态回调函数
    static void closeCient(SocketData* cdata);
    /**
     * @brief closeClientByThread 重要:，在client中，关闭socket意味着需要把libuv的loop也关闭，测试后确认需要开一个独立线程去关闭
     * 如果是ui中调用close关闭socket，可以直接调用close-》closeClient，如果实在loop中，则必须调用closeClientByThread
     * @param client
     * @param cdata
     */
    static void closeClientByThread(SocketData* cdata);

    static void onAfterConnect(uv_connect_t* req, int status);

    static void onAfterRead(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf);
    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

    static void onAfterClientClose(uv_handle_t *handle);
    static void onAfterWrite(uv_write_t *req, int status);

private:

     SocketData* m_socketData;

     string m_remoteIp;
     int m_remotePort;

     //handle需要使用m_socketData的，从AbstractSocket继承来的m_socket忽略
     //uv_tcp_t* handle(){return m_socket;}
};

#endif // TCPCLIENT_H
