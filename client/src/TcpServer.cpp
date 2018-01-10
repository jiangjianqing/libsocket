#include "TcpServer.h"

TcpServer::TcpServer()
    : m_cbConnectionAccpeted(nullptr),m_cbClientClosed(nullptr)
{
    //deprecated 不使用libuv提供的mutex，改用c++ 标准库中的mutex
    //int iret = uv_mutex_init(&m_mutexClients);
    /*
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }*/
}

TcpServer::~TcpServer()
{
    close();

    //deprecated
   // uv_mutex_destroy(&m_mutexClients);
}

bool TcpServer::start( const string& ip, int port )
{
    close();
    if (!init()) {
        return false;
    }

    if (!bind(ip,port)) {
        return false;
    }
    //DEFAULT_BACKLOG 是自己定义的，目前使用libuv定义的SOMAXCONN
    if (!listen(SOMAXCONN)) {
        return false;
    }
    run();
    //LOGI("start listen "<<ip<<": "<<port);
    return true;
}



void TcpServer::close()
{
    if(!m_isInited){
        return;
    }

    for (auto it = m_clients.begin(); it!=m_clients.end(); ++it) {
        SocketData* cdata = it->second;
        //奇怪：为什么onAfterClientClose没有被调用？？？？
        if (uv_is_active((uv_handle_t*)cdata->handle())) {
            uv_read_stop((uv_stream_t*)cdata->handle());
        }
        uv_close((uv_handle_t*)cdata->handle(),onAfterClientClose);
        //delete cdata;
    }
    m_clients.clear();

    uv_close((uv_handle_t*) &m_socket, onAfterServerClose);

        //LOGI("close server");


    AbstractSocket::close();



}

bool TcpServer::bind(const string& ip, int port)
{
    //ip6方式
    //struct sockaddr_in6 bind_addr;
    //int iret = uv_ip6_addr(ip.c_str(), port, &bind_addr);
    struct sockaddr_in bind_addr;
    int iret = uv_ip4_addr(ip.c_str(), port, &bind_addr);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }
    iret = uv_tcp_bind(&m_socket, (const struct sockaddr*)&bind_addr,0);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }
    //LOGI("server bind ip="<<ip<<", port="<<port);
    return true;
}

bool TcpServer::listen(int backlog)
{
    int iret = uv_listen((uv_stream_t*) &m_socket, backlog, onAcceptConnection);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }
    refreshInfo();//刷新一下自己的ip和端口
    //LOGI("server listen");
    return true;
}

void TcpServer::onAcceptConnection(uv_stream_t *server, int status)
{
    if (!server->data) {
        return;
    }
    TcpServer *tcpsock = (TcpServer *)server->data;
    int clientId = tcpsock->getAvailableClientId();
    SocketData* cdata = new SocketData(clientId , tcpsock);//uv_close回调函数中释放
    //cdata->tcp_server = tcpsock;//保存服务器的信息
    int iret = uv_tcp_init(tcpsock->m_loop , cdata->handle());//由析构函数释放
    if (iret) {
        delete cdata;
        tcpsock->setErrMsg(iret);
        return;
    }
    iret = uv_accept(server, (uv_stream_t*) cdata->handle());
    //如果不允许接受连接, 可以在这里(uv_listen 的回调函数中)关闭套接字.
    if ( iret) {
        tcpsock->setErrMsg(iret);
        uv_close((uv_handle_t*) cdata->handle(), NULL);
        delete cdata;
        return;
    }

    cdata->refreshInfo();//刷新一下信息，可以得到客户端的IP和端口
    unique_lock<mutex> lock1(tcpsock->m_mutexClients);//开启互锁
    tcpsock->m_clients.insert(make_pair(clientId,cdata));//加入到链接队列
    if (tcpsock->m_cbConnectionAccpeted) {
        tcpsock->m_cbConnectionAccpeted(cdata->ip(),cdata->port());
    }
    lock1.unlock();


    //LOGI("new client("<<cdata->client_handle<<") id="<< clientid);
    iret = uv_read_start((uv_stream_t*)cdata->handle(), onAllocBuffer, onAfterServerRecv);//服务器开始接收客户端的数据

}

//服务器分析空间函数
void TcpServer::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    if (!handle->data) {
        return;
    }
    SocketData *cdata = (SocketData*)handle->data;
    *buf = cdata->readbuffer;
}

void TcpServer::onAfterServerRecv(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf)
{
    if (!handle->data) {
        return;
    }
    SocketData * cdata = (SocketData*)handle->data;//服务器的recv带的是clientdata
    if (nread < 0) {/* Error or EOF */
        TcpServer *server = (TcpServer *)cdata->socket();
        if (nread == UV_EOF) {
            //fprintf(stdout,"客户端(%d)连接断开，关闭此客户端\n",client->client_id);
            //LOGW("客户端("<<client->client_id<<")主动断开");
        } else if (nread == UV_ECONNRESET) {
            //fprintf(stdout,"客户端(%d)异常断开\n",client->client_id);
            //LOGW("客户端("<<client->client_id<<")异常断开");
        } else {
            //fprintf(stdout,"%s\n",GetUVError(nread));
            //LOGW("客户端("<<client->client_id<<")异常断开："<<GetUVError(nread));
        }
        server->deleteClient(cdata->clientId());//连接断开，关闭客户端
        return;
    } else if (0 == nread)  {/* Everything OK, but nothing read. */

    } else /*if (client->recvcb_)*/ {//正常读取数据
        //uv_as
        //client->recvcb_(client->client_id,buf->base,nread);
    }
}

void TcpServer::onAfterClientClose(uv_handle_t *handle)
{
    SocketData *cdata = (SocketData*)handle->data;
    TcpServer *server = (TcpServer *)cdata->socket();
    if(server->m_cbClientClosed){

        server->m_cbClientClosed(cdata->clientId(),cdata->ip(),cdata->port());
    }
    handle->data = nullptr;
    //LOGI("client "<<cdata->client_id<<" had closed.");
    delete cdata;
}

void TcpServer::onAfterServerClose(uv_handle_t *handle)
{
    //服务器,不需要做什么
    int i = 0;
    i = i + 1;
}

bool TcpServer::deleteClient( int clientId )
{
    unique_lock<mutex> lock1(m_mutexClients);
    //unique_lock<mutex> lock1(m_mutexClients,defer_lock_t());//使用defer_lock_t是为了能够用泛型lock函数，一次锁定多个mutex
    //lock(lock1,lock2);
    //uv_mutex_lock(&m_mutexClients);
    auto itfind = m_clients.find(clientId);
    if (itfind == m_clients.end()) {
        //errmsg_ = "can't find client ";
        //errmsg_ += std::to_string((long long)clientid);
        //LOGE(errmsg_);
        //uv_mutex_unlock(&m_mutexClients);
        return false;
    }
    SocketData* cdata = itfind->second;

    if (uv_is_active((uv_handle_t*)cdata->handle())) {
        uv_read_stop((uv_stream_t*)cdata->handle());
    }

    uv_close((uv_handle_t*)cdata->handle(),onAfterClientClose);

    m_clients.erase(itfind);
    //LOGI("删除客户端"<<clientid);
    //uv_mutex_unlock(&m_mutexClients);
    return true;
}



