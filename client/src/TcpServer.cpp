#include "TcpServer.h"

TcpServer::TcpServer()
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

bool TcpServer::bind(const string& ip, int port)
{
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

bool TcpServer::bind6(const string& ip, int port)
{
    struct sockaddr_in6 bind_addr;
    int iret = uv_ip6_addr(ip.c_str(), port, &bind_addr);
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
    int iret = uv_listen((uv_stream_t*) &m_server, backlog, acceptConnection);
    if (iret) {
        m_errmsg = getUVError(iret);
        //LOGE(errmsg_);
        return false;
    }
    //LOGI("server listen");
    return true;
}

void TcpServer::acceptConnection(uv_stream_t *server, int status)
{
    if (!server->data) {
        return;
    }
    TcpServer *tcpsock = (TcpServer *)server->data;
    int clientId = tcpsock->getAvailableClientId();
    clientdata* cdata = new clientdata(clientId);//uv_close回调函数中释放
    cdata->tcp_server = tcpsock;//保存服务器的信息
    int iret = uv_tcp_init(tcpsock->loop_, cdata->client_handle);//析构函数释放
    if (iret) {
        delete cdata;
        tcpsock->m_errmsg = getUVError(iret);
        //LOGE(tcpsock->errmsg_);
        return;
    }
    iret = uv_accept((uv_stream_t*)&tcpsock->server_, (uv_stream_t*) cdata->client_handle);
    if ( iret) {
        tcpsock->m_errmsg = getUVError(iret);
        uv_close((uv_handle_t*) cdata->client_handle, NULL);
        delete cdata;
        //LOGE(tcpsock->errmsg_);
        return;
    }
    tcpsock->clients_list_.insert(std::make_pair(clientid,cdata));//加入到链接队列
    if (tcpsock->newconcb_) {
        tcpsock->newconcb_(clientid);
    }
    //LOGI("new client("<<cdata->client_handle<<") id="<< clientid);
    iret = uv_read_start((uv_stream_t*)cdata->client_handle, onAllocBuffer, AfterServerRecv);//服务器开始接收客户端的数据
    return;
}
