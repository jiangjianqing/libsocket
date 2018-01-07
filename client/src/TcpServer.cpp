#include "TcpServer.h"

TcpServer::TcpServer() : m_newConnect_cb(nullptr)
{

}

TcpServer::~TcpServer()
{

}

bool TcpServer::connect(const string &ip, int port)
{
    return false;
}

void TcpServer::close()
{

}

//服务器-新链接回调函数
void TcpServer::setNewConnectCb(new_connect_cb cb)
{
    m_newConnect_cb = cb;
}

bool TcpServer::init()
{
    bool ret = AbstractSocket::init();
    if(ret == false){
        return false;
    }

    int iret = uv_mutex_init(&m_mutexClients);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }

    return true;
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
    SocketData* cdata = new SocketData(clientId,tcpsock);//uv_close回调函数中释放
    //cdata->tcp_server = tcpsock;//保存服务器的信息
    int iret = uv_tcp_init(tcpsock->m_loop, cdata->socketHandle());//析构函数释放
    if (iret) {
        delete cdata;
        tcpsock->m_errmsg = getUVError(iret);
        //LOGE(tcpsock->errmsg_);
        return;
    }
    iret = uv_accept((uv_stream_t*)&tcpsock->m_socket, (uv_stream_t*) cdata->socketHandle());
    if ( iret) {
        tcpsock->m_errmsg = getUVError(iret);
        uv_close((uv_handle_t*) cdata->socketHandle(), NULL);
        delete cdata;
        //LOGE(tcpsock->errmsg_);
        return;
    }
    tcpsock->m_clients.insert(std::make_pair(clientId,cdata));//加入到链接队列
    if (tcpsock->m_newConnect_cb) {
        tcpsock->m_newConnect_cb(clientId);
    }
    //LOGI("new client("<<cdata->client_handle<<") id="<< clientid);
    iret = uv_read_start((uv_stream_t*)cdata->socketHandle(), onAllocBuffer, onAfterServerRecv);//服务器开始接收客户端的数据
    return;
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

    }/* else if (client->recvcb_) {
        client->recvcb_(client->client_id,buf->base,nread);
    }*/
}

void TcpServer::onAfterClientClose(uv_handle_t *handle)
{
    SocketData *cdata = (SocketData*)handle->data;
    //LOGI("client "<<cdata->client_id<<" had closed.");
    delete cdata;
}

bool TcpServer::deleteClient( int clientId )
{
    uv_mutex_lock(&m_mutexClients);
    auto itfind = m_clients.find(clientId);
    if (itfind == m_clients.end()) {
        //errmsg_ = "can't find client ";
        //errmsg_ += std::to_string((long long)clientid);
        //LOGE(errmsg_);
        uv_mutex_unlock(&m_mutexClients);
        return false;
    }
    if (uv_is_active((uv_handle_t*)itfind->second->socket())) {
        uv_read_stop((uv_stream_t*)itfind->second->socket());
    }
    uv_close((uv_handle_t*)itfind->second->socket(),onAfterClientClose);

    m_clients.erase(itfind);
    //LOGI("删除客户端"<<clientid);
    uv_mutex_unlock(&m_mutexClients);
    return true;
}
