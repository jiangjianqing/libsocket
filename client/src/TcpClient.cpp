#include "TcpClient.h"

TcpClient::TcpClient()
{

}

TcpClient::~TcpClient()
{
    close();
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

    uv_close((uv_handle_t*) handle(), onAfterClientClose);
    //----最后调用parent的close
    AbstractSocket::close();
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
    m_socketData = new SocketData(-1,this);
    iret = uv_tcp_init(m_loop , handle());//由析构函数释放
    if (iret) {
        //delete cdata;
        //server->setErrMsg(iret);
        return;
    }
    uv_connect_t* connectReq = (uv_connect_t*)malloc(sizeof(uv_connect_t));//连接时请求
    iret = uv_tcp_connect(connectReq, handle(), (const sockaddr*)&bind_addr, onAfterConnect);
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
            //pclient->connectstatus_ = CONNECT_ERROR;
            //fprintf(stdout,"connect error:%s\n",GetUVError(status));

    }else{
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
    SocketData *cdata = (SocketData*)handle->data;
    *buf = cdata->readbuffer;
}

void TcpClient::onAfterRead(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf)
{
    if (!handle->data) {
        return;
    }
    TcpClient *client = (TcpClient*)handle->data;//服务器的recv带的是TCPClient
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
        uv_close((uv_handle_t*)handle, onAfterClientClose);

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

void TcpClient::AfterSend(uv_write_t *req, int status)
{
    TcpClient *client = (TcpClient *)req->handle->data;
    //uv_mutex_unlock(&client->write_mutex_handle_);
    if (status < 0) {
        //LOGE("发送数据有误:"<<GetUVError(status));
        //fprintf(stderr, "Write error %s\n", GetUVError(status));
    }
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
    TcpClient *client = (TcpClient *)req->handle->data;
    //uv_mutex_unlock(&client->write_mutex_handle_);
    if (status < 0) {
        //LOGE("发送数据有误:"<<GetUVError(status));
        //fprintf(stderr, "Write error %s\n", GetUVError(status));
    }
}
