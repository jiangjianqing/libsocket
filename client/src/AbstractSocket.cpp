#include "AbstractSocket.h"

AbstractSocket::AbstractSocket()
{

}

AbstractSocket::~AbstractSocket()
{

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
