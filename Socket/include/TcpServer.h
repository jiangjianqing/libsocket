#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "AbstractSocket.h"
#include "SocketData.h"
#include <map>
#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>


#define DEFAULT_BACKLOG 128

enum class StreamType{
    TCP = 0,
    UDP,
    PIPE
} ;


class TcpServer : public AbstractSocket
{
public:


    explicit TcpServer();
    virtual ~TcpServer();

    bool start( const string& ip, int port );

    void close() override;

    bool send(int clientid, const char* data, std::size_t len);

private:

    shared_timed_mutex m_mutexClients;//保护clients_list_
    std::map<int,handle_data_t*> m_clients;//子客户端链接

    bool bind(const string& ip, int port);
    bool listen(int backlog = DEFAULT_BACKLOG);
    int getAvailableClientId() const{static atomic<int> s_id(0); return ++s_id; }//没有考虑频繁连接、断开导致s_id资源耗尽的情况

    bool removeClient( int clientId );

    static void closeClient(handle_data_t* cdata,bool removeFromClients);
    static void onAcceptConnection(uv_stream_t *server, int status);
    static void onAfterRead(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf);
    static void onAfterShutdown(uv_shutdown_t* req, int status);
    static void onAfterClientClose(uv_handle_t *handle);
    static void onAfterServerClose(uv_handle_t *handle);
    static void onAfterWrite(uv_write_t *req, int status);

};

#endif // TCPSERVER_H
