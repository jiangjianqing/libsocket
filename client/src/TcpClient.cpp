#include "TcpClient.h"
#include <thread>

TcpClient::TcpClient()
{
    m_socketData = new SocketData(-1,this,(uv_handle_t*)malloc(sizeof(uv_tcp_t)));
    //2018.01.11 重要： 为了沿用AbstractSocket中对loop的多线程处理，必须将忽略m_socketData中对于自身handle的处理
    m_socketData->setExternalHandle(handle());
}

TcpClient::~TcpClient()
{
    close();

    delete m_socketData;
}

/*
bool TcpClient::connect(const string &ip, int port)
{
    return false;
}
*/

void TcpClient::close()
{
    if(!m_isInited){
        return;
    }

    closeCient(m_socketData);//closeClient中负责调用AbstractSocket::close();，关闭连接 = 关闭socket
    //----最后调用parent的close
}

//作为client的connect函数

bool TcpClient::connect(const char* ip, int port)
{
    close();
    if(!init()){
        return false;
    }
    ConnectThread(ip,port);
    //connectip_ = ip;
    //connectport_ = port;
    //LOGI("客户端("<<this<<")start connect to server("<<ip<<":"<<port<<")");
    //int iret = uv_thread_create(&connect_threadhanlde_, ConnectThread, this);//触发AfterConnect才算真正连接成功，所以用线程
    //if (iret) {
        //errmsg_ = GetUVError(iret);
        //LOGE(errmsg_);
        //return false;
    //}
/*
    while ( connectstatus_ == CONNECT_DIS) {
#if defined (WIN32) || defined(_WIN32)
        Sleep(100);
#else
        usleep((100) * 1000);
#endif
    }
    */

    //return connectstatus_ == CONNECT_FINISH;

    run();
    return true;
}

void TcpClient::ConnectThread(const char* ip, int port)
{
    //fprintf(stdout,"client(%p) ConnectThread start\n",pclient);
    //struct sockaddr_in6 bind_addr;
    //int iret = uv_ip6_addr(pclient->connectip_.c_str(), pclient->connectport_, &bind_addr);

    struct sockaddr_in bind_addr;
    int iret = uv_ip4_addr(ip, port, &bind_addr);
    if (iret) {
        //pclient->errmsg_ = GetUVError(iret);
        //LOGE(pclient->errmsg_);
            return;
    }
/*
    iret = uv_tcp_init(m_loop , handle());//由析构函数释放
    if (iret) {
        //delete cdata;
        //server->setErrMsg(iret);
        return;
    }*/
    uv_connect_t* connectReq = (uv_connect_t*)malloc(sizeof(uv_connect_t));//连接时请求
    iret = uv_tcp_connect(connectReq, (uv_tcp_t*)handle(), (const sockaddr*)&bind_addr, onAfterConnect);
    if (iret) {
        //pclient->errmsg_ = GetUVError(iret);
        //LOGE(pclient->errmsg_);
        return;
    }

    //fprintf(stdout,"client(%p) ConnectThread end, connect status %d\n",pclient, pclient->connectstatus_);

    //pclient->run();

}

void TcpClient::onAfterConnect(uv_connect_t* req, int status)
{
    //fprintf(stdout,"start after connect\n");
    TcpClient *client = (TcpClient*)req->handle->data;
    if (status) {
        SocketData* cdata = client->m_socketData;
        closeClientByThread(cdata);
        //pclient->connectstatus_ = CONNECT_ERROR;
        //fprintf(stdout,"connect error:%s\n",GetUVError(status));

    }else{
        client->refreshInfo();
        int iret = uv_read_start(req->handle, onAllocBuffer, onAfterRead);//客户端开始接收服务器的数据
        if (iret) {
            //fprintf(stdout,"uv_read_start error:%s\n",GetUVError(iret));
            //client->connectstatus_ = CONNECT_ERROR;

        } else {
            //pclient->connectstatus_ = CONNECT_FINISH;
        }
         //LOGI("客户端("<<pclient<<")run");
         //fprintf(stdout,"end after connect\n");
    }

    free(req);
}

void TcpClient::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    if (!handle->data) {
        return;
    }
    TcpClient* client = static_cast<TcpClient*>(handle->data);
    SocketData *cdata = client->m_socketData;
    *buf = cdata->readbuffer;
}

void TcpClient::onAfterRead(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf)
{
    if (!handle->data) {
        return;
    }
    TcpClient* client = static_cast<TcpClient*>(handle->data);
    SocketData* cdata = client->m_socketData;
    if (nread < 0) {
        if (nread == UV_EOF) {
            //fprintf(stdout,"服务器(%p)主动断开\n",handle);
            //LOGW("服务器主动断开");
        } else if (nread == UV__ECONNRESET) {
            //fprintf(stdout,"服务器(%p)异常断开\n",handle);
            //LOGW("服务器异常断开");
        } else {
            //fprintf(stdout,"服务器(%p)异常断开:%s\n",handle,GetUVError(nread));
            //LOGW("服务器异常断开"<<GetUVError(nread));
        }

        //2018.1.12 重要:，在client中，关闭socket意味着需要把libuv的loop也关闭，测试后确认需要开一个独立线程去关闭
        closeClientByThread(cdata);

        return;

    } else if (0 == nread)  {/* Everything OK, but nothing read. */

    } else /*if (client->recvcb_)*/ {//正常读取数据
        //echo test
        SocketData::reverse(buf->base,nread);
        client->send(buf->base,nread);
    }

}

void TcpClient::onAfterClientClose(uv_handle_t *handle)
{
    //fprintf(stdout,"客户端(%p)已close\n",handle);
    //LOGI("客户端("<<handle<<")已close");
}

bool TcpClient::send(const char* data, std::size_t len)
{

    //自己控制data的生命周期直到write结束
    SocketData* cdata = m_socketData;
    return cdata->send(data,len,&TcpClient::onAfterWrite);
    //return send(cdata,data,len);
}

void TcpClient::onAfterWrite(uv_write_t *req, int status)
{

    //uv_mutex_unlock(&client->write_mutex_handle_);
    if (status < 0) {
        TcpClient* client = static_cast<TcpClient*>(req->handle->data);
        SocketData* cdata = client->m_socketData;

        socket_event_t event(socket_event_type::WriteError,cdata);

        //2018.1.12 重要:，在client中，关闭socket意味着需要把libuv的loop也关闭，测试后确认需要开一个独立线程去关闭
        closeClientByThread(cdata);

        //client->callClientEventCb(clientEvent);
        //m_errmsg = "发送数据有误:"<<getUVError(status);
            //LOGE("发送数据有误:"<<GetUVError(status));

            //fprintf(stderr, "Write error %s\n", GetUVError(status));
    }
}

void TcpClient::closeClientByThread(SocketData* cdata)
{
    thread t1(&TcpClient::closeCient,cdata);
    t1.detach();
}

void TcpClient::closeCient(SocketData* cdata)
{
    TcpClient *client = (TcpClient *)cdata->socket();
    if (uv_is_active((uv_handle_t*)cdata->handle())) {
        uv_read_stop((uv_stream_t*)cdata->handle());
    }

    uv_close((uv_handle_t*)cdata->handle(),onAfterClientClose);

    //关闭连 = 关闭整个socket
    client->AbstractSocket::close();

}

void TcpClient::refreshInfo()
{
    AbstractSocket::refreshInfo();

    struct sockaddr sockname,peername;

    //获得对方 的IP和端口，当没有连接时，peername是无意义的
    int namelen = sizeof peername;

    uv_tcp_getpeername((uv_tcp_t*)handle(), &peername, &namelen);
    m_remoteIp = string(inet_ntoa(((sockaddr_in *)&peername)->sin_addr));
    m_remotePort = ntohs(((sockaddr_in *)&peername)->sin_port);

}
