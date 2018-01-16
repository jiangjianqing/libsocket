#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "uv.h"
#include "SocketData.h"
#include "AbstractBufferPool.h"


using namespace std;

class AbstractSocket
{
public:
    AbstractSocket(SocketType type);
    explicit AbstractSocket();
    virtual ~AbstractSocket();
    //virtual bool connect(const string& ip,int port) = 0;

    //是否启用Nagle算法
    bool setNoDelay(bool enable);
    bool setKeepAlive(int enable, unsigned int delay);

    static string getUVError(int retcode);

    virtual void close();

    bool isRunning(){return m_isRunning;}

    void callSocketEventCb(const socket_event_t& event);

    void setSocketEventCb(socket_event_cb cb){m_cbSocketEvent = cb;}
    uv_handle_t* handle();
    SocketType socketType(){return m_socketType;}

    void setBufPool(AbstractBufferPool* pool);


protected:


    string m_ip;
    int m_port;
    uv_loop_t *m_loop;
    uv_tcp_t m_tcpHandle;
    uv_udp_t m_udpHandle;
    string m_errmsg;
    bool m_isInited;
    uv_idle_t m_idler;

    void setErrMsg(int uvErrId);
    void run(int mode = UV_RUN_DEFAULT);



    virtual bool init();
    /**
     * @brief deinit 直接调用close，但含义不同，deinit表示完成了init，但后续的run并没有成功运行
     */
    virtual void deinit();

    virtual void refreshInfo();

    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    void freeBuf(const uv_buf_t* buf);

private:
    static void onIdle(uv_idle_t *handle);

    AbstractBufferPool* m_bufPool;
    SocketType m_socketType;
    mutex m_mutexRun;
    condition_variable m_condVarRun;
    bool m_isRunning;
    socket_event_cb m_cbSocketEvent;
};

#endif // ABSTRACTSOCKET_H
