#ifndef SOCKETDATA_H
#define SOCKETDATA_H

#include "uv.h"
#include <functional>

using namespace std;
class AbstractSocket;

//注意：这是非常有用的调试函数
#define FATAL(msg)                                        \
  do {                                                    \
    fprintf(stderr,                                       \
            "Fatal error in %s on line %d: %s\n",         \
            __FILE__,                                     \
            __LINE__,                                     \
            msg);                                         \
    fflush(stderr);                                       \
    abort();                                              \
  } while (0)

#define INFO(msg)                                        \
  do {                                                    \
    fprintf(stdout,                                       \
            "INFO on line %d: %s : %s \n",         \
            __LINE__,                                     \
            __FILE__,                                     \
            msg);                                         \
    fflush(stdout);                                       \
  } while (0)

typedef void (*server_recvcb)(int cliendid, const char* buf, int bufsize);
typedef void (*client_recvcb)(const char* buf, int bufsize, void* userdata);



class SocketData
{
public:
    typedef struct {
      uv_write_t req;
      uv_buf_t buf;
    } write_req_t;

    friend class AbstractSocket;
    explicit SocketData(int clientid,AbstractSocket* socket);
    virtual ~SocketData();

    AbstractSocket* socket(){return m_socket;}

    uv_tcp_t* handle(){return m_socketHandle;}

    /**
     * @brief setExternalHandle //2018.01.11 重要： TcpClient为了沿用AbstractSocket中对loop的多线程处理，必须将忽略m_socketData中对于自身handle的处理
     * @param handle
     */
    void setExternalHandle(uv_tcp_t* handle);

    int clientId(){return m_clientId;}
    const string& ip(){return m_ip;}
    int port(){return m_port;}

    uv_buf_t readbuffer;//接受数据的buf

    void refreshInfo();
    bool send(const char* data, size_t len,uv_write_cb cb);

    /**
     * @brief reverse 反转字符串，用于echo测试
     * @param s
     * @param len
     */
    static void reverse(char *s, int len);

protected:
    uv_tcp_t* m_socketHandle;//客户端句柄
private:
    int m_clientId;//客户端id,惟一

    bool m_useExternalHandle;

    string m_ip;
    int m_port;

    AbstractSocket* m_socket;//服务器句柄(保存是因为某些回调函数需要到)

    uv_buf_t writebuffer;//写数据的buf
    uv_write_t write_req;
};


enum class socket_event_type{
    Unknow,ReadError,WriteError,ConnectionAccept,ConnectionClose,Listening,ListenFault,SocketClose
};

struct socket_event_s{
    socket_event_type type;
    int clientId;
    string ip;
    int port;
    string msg;

    socket_event_s(socket_event_type eventType,SocketData* cdata){
        clientId = cdata->clientId();
        ip = cdata->ip();
        port = cdata->port();
        type = eventType;
    }
    socket_event_s(socket_event_type eventType = socket_event_type::Unknow){
        type = eventType;
    }
};
typedef socket_event_s socket_event_t;

typedef function<void (const string& ip,int port)> connection_accepted_cb;
typedef function<void (int id, const string& ip,int port)> client_close_cb;
typedef function<void (const socket_event_t& event)> socket_event_cb;

#endif // SOCKETDATA_H
