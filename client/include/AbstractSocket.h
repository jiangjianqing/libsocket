#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "uv.h"
#include "SocketData.h"

using namespace std;

class AbstractSocket
{
public:
    explicit AbstractSocket();
    virtual ~AbstractSocket();
    //virtual bool connect(const string& ip,int port) = 0;

    //是否启用Nagle算法
    bool setNoDelay(bool enable);
    bool setKeepAlive(int enable, unsigned int delay);

    static string getUVError(int retcode);

    virtual void close();

    bool isRuning(){return m_isRuning;}

    void callSocketEventCb(const socket_event_t& event);

    void setSocketEventCb(socket_event_cb cb){m_cbSocketEvent = cb;}

protected:
    string m_ip;
    int m_port;
    uv_loop_t *m_loop;
    uv_tcp_t m_socket;
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

private:
    static void onIdle(uv_idle_t *handle);
    mutex m_mutexRun;
    condition_variable m_condVarRun;
    bool m_isRuning;
    socket_event_cb m_cbSocketEvent;
};

#endif // ABSTRACTSOCKET_H
