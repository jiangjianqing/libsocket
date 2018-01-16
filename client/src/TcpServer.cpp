#include "TcpServer.h"

TcpServer::TcpServer()
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

    uv_close(handle(), onAfterServerClose);

    for (auto it = m_clients.begin(); it!=m_clients.end(); ++it) {
        SocketData* cdata = it->second;
        closeClient(cdata,false);
    }
    m_clients.clear();

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
    iret = uv_tcp_bind((uv_tcp_t*)handle(), (const struct sockaddr*)&bind_addr,0);
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
    socket_event_t event;
    bool ret = false;
    int iret = uv_listen((uv_stream_t*)handle(), backlog, onAcceptConnection);
    if (iret) {
        ret = false;
        m_errmsg = getUVError(iret);
        deinit();//注意：调用deinit虽然功能与close相同，但表示初始化过程中失败了
        INFO("TcpServer 创建Listen 失败！");
        //LOGE(errmsg_);
        event.type = socket_event_type::ListenFault;

    }else{
        ret = true;
        refreshInfo();//刷新一下自己的ip和端口
        event.type = socket_event_type::Listening;
    }

    callSocketEventCb(event);

    return ret;

}

void TcpServer::onAcceptConnection(uv_stream_t *stream, int status)
{
    if (!stream->data) {
        return;
    }
    TcpServer *server = (TcpServer *)stream->data;
    int clientId = server->getAvailableClientId();
    SocketData* cdata = new SocketData(clientId , server,(uv_handle_t*)malloc(sizeof(uv_tcp_t)));//uv_close回调函数中释放
    //cdata->tcp_server = tcpsock;//保存服务器的信息
    int iret = uv_tcp_init(server->m_loop , (uv_tcp_t*)cdata->handle());//由析构函数释放
    if (iret) {
        INFO("创建client connect 失败");
        delete cdata;
        server->setErrMsg(iret);
        return;
    }
    iret = uv_accept(stream, (uv_stream_t*) cdata->handle());
    //如果不允许接受连接, 可以在这里(uv_listen 的回调函数中)关闭套接字.
    if ( iret) {
        server->setErrMsg(iret);
        uv_close((uv_handle_t*) cdata->handle(), NULL);
        delete cdata;
        return;
    }

    cdata->refreshInfo();//刷新一下信息，可以得到客户端的IP和端口

    unique_lock<shared_timed_mutex> lock1(server->m_mutexClients);//开启互锁
    server->m_clients.insert(make_pair(clientId,cdata));//加入到链接队列

    socket_event_t event(socket_event_type::ConnectionAccept,cdata);
    lock1.unlock();

    server->callSocketEventCb(event);

    //LOGI("new client("<<cdata->client_handle<<") id="<< clientid);
    iret = uv_read_start((uv_stream_t*)cdata->handle(), onAllocBuffer, onAfterRead);//服务器开始接收客户端的数据

}

void TcpServer::onAfterRead(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf)
{
    if (!handle->data) {
        return;
    }
    SocketData * cdata = (SocketData*)handle->data;//服务器的recv带的是clientdata
    if (nread < 0) {/*客户端断开连接时 */
        //先保存下错误信息

        socket_event_t event(socket_event_type::ReadError,cdata);

        if (nread == UV_EOF) {
            event.type = socket_event_type::ConnectionClose;
            //fprintf(clientError.msg,"客户端(%d)连接断开，关闭此客户端\n",clientError.clientId);
            //fprintf(stdout,"客户端(%d)连接断开，关闭此客户端\n",client->client_id);
            //LOGW("客户端("<<client->client_id<<")主动断开");
        } else if (nread == UV__ECONNRESET) { //UV__ECONNREFUSED
            //fprintf(stdout,"客户端(%d)异常断开\n",client->client_id);
            //LOGW("客户端("<<client->client_id<<")异常断开");
        } else {
            //fprintf(stdout,"%s\n",GetUVError(nread));
            //LOGW("客户端("<<client->client_id<<")异常断开："<<GetUVError(nread));
        }
        uv_shutdown_t* req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
        uv_shutdown(req, handle, onAfterShutdown);

        //-----shutdown之后再去触发callback，callback中不要从clients获取任何信息，因为此时不可靠
        TcpServer *server = (TcpServer *)cdata->socket();
        server->callSocketEventCb(event);


        return;
    } else if (0 == nread)  {/* Everything OK, but nothing read. */

    } else /*if (client->recvcb_)*/ {//正常读取数据
        //echo test
        SocketData::reverse(buf->base,nread);
        cdata->send(buf->base,nread,&TcpServer::onAfterWrite);
        //uv_as
        //client->recvcb_(client->client_id,buf->base,nread);
    }
}

void TcpServer::onAfterClientClose(uv_handle_t *handle)
{
    SocketData *cdata = (SocketData*)handle->data;
    //TcpServer *server = (TcpServer *)cdata->socket();
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

void TcpServer::onAfterShutdown(uv_shutdown_t* req, int status)
{
    //uv_close((uv_handle_t*)req->handle, on_close);
    SocketData * cdata = (SocketData*)req->handle->data;
    closeClient(cdata,true);//先关闭连接，再移除clients
    free(req);
}

void TcpServer::onAfterWrite(uv_write_t *req, int status)
{
    //m_cbAfterWrited(req,status);
    if (status == 0)
        return;

    if (status < 0) {//连接断开，关闭客户端
        SocketData * cdata = (SocketData*)req->handle->data;
        TcpServer *server = (TcpServer *)cdata->socket();
        socket_event_t event(socket_event_type::WriteError,cdata);
        closeClient(cdata,true);//先关闭连接，再移除clients

        server->callSocketEventCb(event);
        //m_errmsg = "发送数据有误:"<<getUVError(status);
            //LOGE("发送数据有误:"<<GetUVError(status));

            //fprintf(stderr, "Write error %s\n", GetUVError(status));
    }

}

void TcpServer::closeClient(SocketData* cdata,bool removeFromClients)
{
    TcpServer *server = (TcpServer *)cdata->socket();
    if (uv_is_active((uv_handle_t*)cdata->handle())) {
        uv_read_stop((uv_stream_t*)cdata->handle());
    }

    uv_close((uv_handle_t*)cdata->handle(),onAfterClientClose);

    if(removeFromClients){
        int clientId = cdata->clientId();
        server->removeClient(clientId);
    }
}

bool TcpServer::removeClient( int clientId )
{
    unique_lock<shared_timed_mutex> lock1(m_mutexClients);
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
    //SocketData* cdata = itfind->second;

    m_clients.erase(itfind);
    //LOGI("删除客户端"<<clientid);
    //uv_mutex_unlock(&m_mutexClients);
    return true;
}

//服务器发送函数

bool TcpServer::send(int clientid, const char* data, std::size_t len)
{
    shared_lock<shared_timed_mutex> lock1(m_mutexClients);
    auto itfind = m_clients.find(clientid);
    if (itfind == m_clients.end()) {
        m_errmsg = "can't find cliendid ";
        m_errmsg += std::to_string((long long)clientid);
           //LOGE(errmsg_);
           return -1;
    }
    //自己控制data的生命周期直到write结束
    SocketData* cdata = itfind->second;
    return cdata->send(data,len,&TcpServer::onAfterWrite);
    //return send(cdata,data,len);
}



