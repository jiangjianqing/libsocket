#include "AbstractSocket.h"
#include <thread>

AbstractSocket::AbstractSocket()
    :m_isInited(false),m_idler(nullptr),m_loop(nullptr)
{



    //
    //uv_loop_t* loop = uv_default_loop()
}

AbstractSocket::~AbstractSocket()
{
    close();
    for(auto itor = m_ptrList.begin(); itor != m_ptrList.end(); itor++){
        void* p = *itor;
        free(p);
    }
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

void AbstractSocket::close()
{
    /*
    if(m_loop != nullptr){
        std::this_thread::sleep_for(chrono::milliseconds(500));
    }*/

    if(m_idler != nullptr){
        uv_idle_stop(m_idler);
        //free(m_idler);
        m_idler = nullptr;
    }
    if(m_loop != nullptr){
        int iret = uv_loop_close(m_loop);
        if (UV_EBUSY == iret){
            int i = 0;
            i = i + 1;
        }
        int activeCounat = uv_loop_alive(m_loop);
/*
        while(true){
            if(UV_EBUSY != uv_loop_close(m_loop)){
                break;
            }
            uv_stop(m_loop);
            std::this_thread::sleep_for(chrono::milliseconds(200));
        }
*/
        //free(m_loop);
        m_loop = nullptr;
    }
}

//初始化与关闭--服务器与客户端一致
bool AbstractSocket::init()
{
    if (m_isInited) {
        return true;
    }

    close();
    m_loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
    m_ptrList.push_back(m_loop);

    int iret = uv_loop_init(m_loop);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }

    iret = uv_tcp_init(m_loop,&m_socket);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }

    //2018.01.09 在TcpServer的close方法中无法用uv_stop退出uv_run，其停留在uv__io_pool函数调用, 最终发现在loop中加入一个idle能克服该问题
    //也许nodejs也在其中默认加入了idle,所以发现不了该问题
    //uv_idle_stop(m_idler); //停止监听，事件环停止，程序停止

    m_idler = (uv_idle_t*)malloc(sizeof(uv_idle_t));
    m_ptrList.push_back(m_idler);
    uv_idle_init(m_loop,m_idler);  //初始化监视器
    uv_idle_start(m_idler, onIdle);  //设置监视器的回调函数

    m_isInited = true;
    m_socket.data = this;
    //iret = uv_tcp_keepalive(&server_, 1, 60);//调用此函数后后续函数会调用出错
    //if (iret) {
    //  errmsg_ = GetUVError(iret);
    //  return false;
    //}
    return true;
}

void AbstractSocket::onIdle(uv_idle_t *handle)
{

}
