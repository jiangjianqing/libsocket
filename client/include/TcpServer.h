#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "AbstractSocket.h"
#include "SocketData.h"
#include <map>

class TcpServer : public AbstractSocket
{
public:
    explicit TcpServer();
    virtual ~TcpServer();

    bool connect(const string &ip, int port) override;
    void close() override;

    bool init() override;
    void setNewConnectCb(new_connect_cb cb);

private:
    new_connect_cb m_newConnect_cb;
    uv_mutex_t m_mutexClients;//保护clients_list_
    std::map<int,SocketData*> m_clients;//子客户端链接

    bool bind(const string& ip, int port);
    bool listen(int backlog);
    int getAvailableClientId() const{static int s_id = 0; return ++s_id; }

    bool deleteClient( int clientId );

    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void onAcceptConnection(uv_stream_t *server, int status);
    static void onAfterServerRecv(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf);
    static void onAfterClientClose(uv_handle_t *handle);

};

#endif // TCPSERVER_H
