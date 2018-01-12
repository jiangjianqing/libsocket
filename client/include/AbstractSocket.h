#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "uv.h"

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

    bool isInited(){return m_isInited;}

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

    virtual void refreshInfo();

private:
    static void onIdle(uv_idle_t *handle);
    mutex m_mutexRun;
    condition_variable m_condVarRun;
};

#endif // ABSTRACTSOCKET_H
