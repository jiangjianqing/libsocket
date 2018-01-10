#ifndef SOCKETDATA_H
#define SOCKETDATA_H

#include "uv.h"
#include "AbstractSocket.h"
#include <functional>

using namespace std;

typedef void (*server_recvcb)(int cliendid, const char* buf, int bufsize);
typedef void (*client_recvcb)(const char* buf, int bufsize, void* userdata);

typedef function<void (const string& ip,int port)> connection_accepted_cb;
typedef function<void (int id, const string& ip,int port)> client_close_cb;

class SocketData
{
public:
    friend class AbstractSocket;
    SocketData(int clientid,AbstractSocket* socket);
    virtual ~SocketData();

    AbstractSocket* socket(){return m_socket;}

    uv_tcp_t* handle(){return m_socketHandle;}

    int clientId(){return m_clientId;}
    string ip(){return m_ip;}
    int port(){return m_port;}

    uv_buf_t readbuffer;//接受数据的buf

    void refreshInfo();

protected:
    uv_tcp_t* m_socketHandle;//客户端句柄
private:
    int m_clientId;//客户端id,惟一

    string m_ip;
    int m_port;

    AbstractSocket* m_socket;//服务器句柄(保存是因为某些回调函数需要到)

    uv_buf_t writebuffer;//写数据的buf
    uv_write_t write_req;
    server_recvcb recvcb_;//接收数据回调给用户的函数
};

#endif // SOCKETDATA_H
