#include "AbstractSocket.h"

AbstractSocket::AbstractSocket()
    :m_isInited(false)
{
    m_loop = uv_default_loop();
    //uv_loop_t* loop = uv_default_loop()
}

AbstractSocket::~AbstractSocket()
{

}

void AbstractSocket::setErrMsg(int uvErrId)
{
    m_errmsg = getUVError(uvErrId);
    //LOGE(tcpsock->errmsg_);
}

string AbstractSocket::getUVError(int retcode)
{
    string err;
    err = uv_err_name(retcode);
    err +=":";
    err += uv_strerror(retcode);
    return std::move(err);
}

bool AbstractSocket::setNoDelay(bool enable)
{
    int iret = uv_tcp_nodelay(&m_socket, enable ? 1 : 0);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }
    return true;
}

bool AbstractSocket::setKeepAlive(int enable, unsigned int delay)
{
    int iret = uv_tcp_keepalive(&m_socket, enable , delay);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }
    return true;
}

//初始化与关闭--服务器与客户端一致
bool AbstractSocket::init()
{
    if (m_isInited) {
        return true;
    }

    if (!m_loop) {
        m_errmsg = "loop is null on tcp init.";
        //LOGE(errmsg_);
        return false;
    }

    int iret = uv_tcp_init(m_loop,&m_socket);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }
    m_isInited = true;
    m_socket.data = this;
    //iret = uv_tcp_keepalive(&server_, 1, 60);//调用此函数后后续函数会调用出错
    //if (iret) {
    //  errmsg_ = GetUVError(iret);
    //  return false;
    //}
    return true;
}
