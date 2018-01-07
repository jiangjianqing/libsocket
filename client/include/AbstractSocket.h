#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include <string>

#include "uv.h"

using namespace std;

class AbstractSocket
{
public:
    explicit AbstractSocket();
    virtual ~AbstractSocket();
    virtual bool connect(const string& ip,int port) = 0;
    virtual void close() = 0;

    //是否启用Nagle算法
    bool setNoDelay(bool enable);
    bool setKeepAlive(int enable, unsigned int delay);

    static string getUVError(int retcode);

    virtual bool init();
protected:
    uv_loop_t *m_loop;
    uv_tcp_t m_socket;
    string m_errmsg;
    bool m_isInited;
};

#endif // ABSTRACTSOCKET_H
