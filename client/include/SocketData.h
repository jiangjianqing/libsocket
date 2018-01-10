#ifndef SOCKETDATA_H
#define SOCKETDATA_H

#include "uv.h"
#include "AbstractSocket.h"

typedef void (*new_connect_cb)(int clientid);
typedef void (*server_recvcb)(int cliendid, const char* buf, int bufsize);
typedef void (*client_recvcb)(const char* buf, int bufsize, void* userdata);

class SocketData
{
public:
    friend class AbstractSocket;
    SocketData(int clientid,AbstractSocket* socket);
    virtual ~SocketData();

    AbstractSocket* socket(){return m_socket;}

    uv_tcp_t* handle(){return m_socketHandle;}

    int clientId(){return m_clientId;}

    uv_buf_t readbuffer;//接受数据的buf

    void refreshInfo();

protected:
    uv_tcp_t* m_socketHandle;//客户端句柄
private:
    int m_clientId;//客户端id,惟一

    AbstractSocket* m_socket;//服务器句柄(保存是因为某些回调函数需要到)

    uv_buf_t writebuffer;//写数据的buf
    uv_write_t write_req;
    server_recvcb recvcb_;//接收数据回调给用户的函数
};

#endif // SOCKETDATA_H
