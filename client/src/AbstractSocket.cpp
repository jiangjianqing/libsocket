#include "AbstractSocket.h"
#include <thread>
#include <string.h>

AbstractSocket::AbstractSocket()
    :m_isInited(false),m_loop(nullptr)
{



    //
    //uv_loop_t* loop = uv_default_loop()
}

AbstractSocket::~AbstractSocket()
{
    close();
    /*
    for(auto itor = m_ptrList.begin(); itor != m_ptrList.end(); itor++){
        void* p = *itor;
        free(p);
    }*/
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

void AbstractSocket::run(int mode)
{
    //LOGI("server runing.");
    int iret;
    thread t1([this,mode](){
        int iret = uv_run(m_loop,(uv_run_mode)mode);
        if (iret) {
            m_errmsg = getUVError(iret);
        }
        m_condVarRun.notify_all();
        int i = 0;
        i = i + 1;
    });
    t1.detach();//脱离主线程的绑定，主线程挂了，子线程不报错，子线程执行完自动退出。
    //detach以后，子线程会成为孤儿线程，线程之间将无法通信。

    //return true;
}

void AbstractSocket::close()
{
    unique_lock<mutex> lock1(m_mutexRun);

    if(m_loop != nullptr){
        uv_idle_stop(&m_idler);
        //stop以后不能马上执行uv_loop_close()，等条件变量触发后再free掉loop指针
        uv_stop(m_loop);
        //std::this_thread::sleep_for(chrono::milliseconds(200));
        int activeCounat = uv_loop_alive(m_loop);
        int iret = uv_loop_close(m_loop);
        m_condVarRun.wait(lock1);
        //int activeCounat = uv_loop_alive(m_loop);
        //int iret = uv_loop_close(m_loop);
        if (UV_EBUSY == iret){//2018.01.10 为什么总是返回UV_EBUSY错误
            int i = 0;
            i = i + 1;
        }

        free(m_loop);
        m_loop = nullptr;
    }
    m_isInited = false;
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

    close();
    m_loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
    //m_ptrList.push_back(m_loop);

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

    //m_idler = (uv_idle_t*)malloc(sizeof(uv_idle_t));
    //m_ptrList.push_back(m_idler);
    uv_idle_init(m_loop,&m_idler);  //初始化监视器
    uv_idle_start(&m_idler, onIdle);  //设置监视器的回调函数

    m_isInited = true;
    m_socket.data = this;
    //iret = uv_tcp_keepalive(&server_, 1, 60);//调用此函数后后续函数会调用出错
    //if (iret) {
    //  errmsg_ = GetUVError(iret);
    //  return false;
    //}
    return true;
}

void AbstractSocket::refreshInfo()
{
    struct sockaddr sockname,peername;
    int namelen;
    int r;

    //获得自己监听的ip和端口
    memset(&sockname, -1, sizeof sockname);
    namelen = sizeof sockname;
    r = uv_tcp_getsockname(&m_socket, &sockname, &namelen);

    m_ip = string(inet_ntoa(((sockaddr_in *)&sockname)->sin_addr));
    m_port = ntohs(((sockaddr_in *)&sockname)->sin_port);

    //获得对方 的IP和端口，当没有连接时，peername是无意义的
    /*
    namelen = sizeof peername;

    uv_tcp_getpeername(&m_socket, &peername, &namelen);
    string ip(inet_ntoa(((sockaddr_in *)&peername)->sin_addr));
    int port = ntohs(((sockaddr_in *)&peername)->sin_port);
    */

    //check_sockname(&sockname, "0.0.0.0", server_port, "server socket");
}

void AbstractSocket::onIdle(uv_idle_t *handle)
{

}
