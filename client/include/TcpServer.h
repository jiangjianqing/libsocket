#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "AbstractSocket.h"
#include "SocketData.h"
#include <map>
#include <atomic>
#include <thread>
#include <mutex>


#define DEFAULT_BACKLOG 128

class TcpServer : public AbstractSocket
{
public:
    explicit TcpServer();
    virtual ~TcpServer();

    bool start( const string& ip, int port );

    void close() override;

    void setNewConnectCb(new_connect_cb cb);
    void setConnectionAcceptedCb(connection_accepted_cb cb){m_cbConnectionAccpeted = cb;}

private:
    connection_accepted_cb m_cbConnectionAccpeted;
    new_connect_cb m_newConnect_cb;
    mutex m_mutexClients;//保护clients_list_
    std::map<int,SocketData*> m_clients;//子客户端链接

    bool bind(const string& ip, int port);
    bool listen(int backlog = DEFAULT_BACKLOG);
    int getAvailableClientId() const{static atomic<int> s_id(0); return ++s_id; }//没有考虑频繁连接、断开导致s_id资源耗尽的情况

    bool deleteClient( int clientId );

    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void onAcceptConnection(uv_stream_t *server, int status);
    static void onAfterServerRecv(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf);
    static void onAfterClientClose(uv_handle_t *handle);
    static void onAfterServerClose(uv_handle_t *handle);


};

#endif // TCPSERVER_H
