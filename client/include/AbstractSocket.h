#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include <string>
#include <vector>

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

    virtual bool init();
protected:
    uv_loop_t *m_loop;
    uv_tcp_t m_socket;
    string m_errmsg;
    bool m_isInited;
    uv_idle_t* m_idler;

    void setErrMsg(int uvErrId);

private:
    static void onIdle(uv_idle_t *handle);
    vector<void*> m_ptrList;
};

#endif // ABSTRACTSOCKET_H
