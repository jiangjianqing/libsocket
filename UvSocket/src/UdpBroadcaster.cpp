#include "UdpBroadcaster.h"
#include "log4z.h"
#define MAXLISTSIZE 20

namespace uv
{
UdpClient::UdpClient()
    : recvcb_(nullptr), recvcb_userdata_(nullptr), closedcb_(nullptr), closedcb_userdata_(nullptr)
    , connectstatus_(CONNECT_DIS), write_circularbuf_(BUFFER_SIZE)
    , isclosed_(true), isuseraskforclosed_(false)
    , isIPv6_(false)
{
    client_handle_ = AllocUdpClientCtx(this);
    int iret = uv_loop_init(&loop_);
    if (iret) {
        errmsg_ = GetUVError(iret);
        LOGE(errmsg_);
        fprintf(stdout, "init loop error: %s\n", errmsg_.c_str());
    }
    iret = uv_mutex_init(&mutex_writebuf_);
    if (iret) {
        errmsg_ = GetUVError(iret);
        LOGE(errmsg_);
    }
    connect_req_.data = this;
}

UdpClient::~UdpClient()
{
    close();
    uv_thread_join(&connect_threadhandle_);
    FreeUdpClientCtx(client_handle_);
    uv_loop_close(&loop_);
    uv_mutex_destroy(&mutex_writebuf_);
    for (auto it = writeparam_list_.begin(); it != writeparam_list_.end(); ++it) {
        FreeWriteParam(*it);
    }
    writeparam_list_.clear();

    LOGI("udpbroadcaster(" << this << ")exit");
}

bool UdpClient::init()
{
    if (!isclosed_) {
        return true;
    }
    int iret = uv_async_init(&loop_, &async_handle_, AsyncCB);
    if (iret) {
        errmsg_ = GetUVError(iret);
        LOGE(errmsg_);
        return false;
    }
    async_handle_.data = this;

    iret = uv_udp_init(&loop_, &client_handle_->udphandle);
    if (iret) {
        errmsg_ = GetUVError(iret);
        LOGE(errmsg_);
        return false;
    }
    client_handle_->udphandle.data = client_handle_;
    client_handle_->parent_server = this;

    client_handle_->packet_->SetPacketCB(GetPacket, client_handle_);
    //client_handle_->packet_->Start(PACKET_HEAD, PACKET_TAIL);//2018.01.22
/*
    iret = uv_timer_init(&loop_, &reconnect_timer_);
    if (iret) {
        errmsg_ = GetUVError(iret);
        LOGE(errmsg_);
        return false;
    }
    reconnect_timer_.data = this;
    */
    LOGI("client(" << this << ")Init");
    isclosed_ = false;
    return true;
}

void UdpClient::closeinl()
{
    if (isclosed_) {
        return;
    }
    uv_walk(&loop_, CloseWalkCB, this);
    LOGI("client(" << this << ")close");
}

bool UdpClient::run(int status)
{
    int iret = uv_run(&loop_, (uv_run_mode)status);
    isclosed_ = true;
    LOGI("client had closed.");
    if (closedcb_) {//trigger close cb to user
        closedcb_(-1, closedcb_userdata_); //client id is -1.
    }
    if (iret) {
        errmsg_ = GetUVError(iret);
        LOGE(errmsg_);
        return false;
    }
    return true;
}

bool UdpClient::connect(const char* ip, int port)
{
    closeinl();
    if (!init()) {
        return false;
    }
    isBroadcast_ = (ip == nullptr);
    connectip_ = isBroadcast_? "0.0.0.0" : ip;
    connectport_ = port;
    isIPv6_ = false;
    struct sockaddr_in bind_addr;
    int iret = 0;
/*
    if(isBroadcast_){
        iret = uv_ip4_addr(connectip_.c_str(), 0, &bind_addr);
    }else{

    }*/
    iret = uv_ip4_addr(connectip_.c_str(), connectport_, &bind_addr);
    if (iret) {
        errmsg_ = GetUVError(iret);
        LOGE(errmsg_);
        return false;
    }
    iret = uv_udp_bind(&client_handle_->udphandle, (const struct sockaddr *)&bind_addr, UV_UDP_REUSEADDR);
    if(isBroadcast_){
        iret = uv_udp_set_broadcast(&client_handle_->udphandle, 1);
    }

    iret = uv_udp_recv_start(&client_handle_->udphandle, AllocBufferForRecv, AfterRecv);

    LOGI("client(" << this << ")start udp (" << connectip_ << ":" << port << ")");
    iret = uv_thread_create(&connect_threadhandle_, ConnectThread, this);//thread to wait for succeed connect.
    if (iret) {
        errmsg_ = GetUVError(iret);
        LOGE(errmsg_);
        return false;
    }
    int wait_count = 0;
    while (connectstatus_ == CONNECT_DIS) {
        ThreadSleep(100);
        if (++wait_count > 100) {
            connectstatus_ = CONNECT_TIMEOUT;
            break;
        }
    }
    if (CONNECT_FINISH != connectstatus_) {
        errmsg_ = "connect time out";
        return false;
    } else {
        return true;
    }
}

void UdpClient::AllocBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    UdpClientCtx* theclass = (UdpClientCtx*)handle->data;
    assert(theclass);
    *buf = theclass->read_buf_;
}

void UdpClient::AfterRecv(uv_udp_t* handle
                               ,ssize_t nread,
                               const uv_buf_t* buf,
                               const struct sockaddr* addr,
                               unsigned flags)
{
    UdpClientCtx* theclass = (UdpClientCtx*)handle->data;
    assert(theclass);
    UdpClient* parent = (UdpClient*)theclass->parent_server;
    if (nread < 0) {/*
        if (parent->reconnectcb_) {
            parent->reconnectcb_(NET_EVENT_TYPE_DISCONNECT, parent->reconnect_userdata_);
        }
        if (!parent->startReconnect()) {
            fprintf(stdout, "Start Reconnect Failure.\n");
            return;
        }*/
        if (nread == UV_EOF) {
            fprintf(stdout, "Server close(EOF), Client %p\n", handle);
            LOGW("Server close(EOF)");
        } else if (nread == UV_ECONNRESET) {
            fprintf(stdout, "Server close(conn reset),Client %p\n", handle);
            LOGW("Server close(conn reset)");
        } else {
            fprintf(stdout, "Server close,Client %p:%s\n", handle, GetUVError(nread));
            LOGW("Server close" << GetUVError(nread));
        }
        uv_close((uv_handle_t*)handle, AfterClientClose);//close before reconnect
        return;
    }
    parent->send_inl(NULL);
    if (nread > 0) {
        theclass->packet_->recvdata((const unsigned char*)buf->base, nread);
    }
}

void UdpClient::AfterClientClose(uv_handle_t* handle)
{
    UdpClient* theclass = (UdpClient*)handle->data;
    fprintf(stdout, "Close CB handle %p\n", handle);
    /*
    if (handle == (uv_handle_t*)&theclass->client_handle_->udphandle && theclass->isreconnecting_) {
        //closed, start reconnect timer
        int iret = 0;
        iret = uv_timer_start(&theclass->reconnect_timer_, TcpClient::ReconnectTimer, theclass->repeat_time_, theclass->repeat_time_);
        if (iret) {
            uv_close((uv_handle_t*)&theclass->reconnect_timer_, TcpClient::AfterClientClose);
            LOGE(GetUVError(iret));
            return;
        }
    }
    */
}

void UdpClient::ConnectThread(void* arg)
{
    UdpClient* pclient = (UdpClient*)arg;
    pclient->run();
}

void UdpClient::close()
{
    if (isclosed_) {
        return;
    }
    isuseraskforclosed_ = true;
    uv_async_send(&async_handle_);
}

int UdpClient::send(const char* data, std::size_t len)
{
    if (!data || len <= 0) {
        errmsg_ = "send data is null or len less than zero.";
        LOGE(errmsg_);
        return 0;
    }
    uv_async_send(&async_handle_);
    size_t iret = 0;
    while (!isuseraskforclosed_) {
        uv_mutex_lock(&mutex_writebuf_);
        iret += write_circularbuf_.write(data + iret, len - iret);
        uv_mutex_unlock(&mutex_writebuf_);
        if (iret < len) {
            ThreadSleep(100);
            continue;
        } else {
            break;
        }
    }
    uv_async_send(&async_handle_);
    return iret;
}

void UdpClient::send_inl(uv_write_t* req /*= NULL*/)
{
    write_param* writep = (write_param*)req;
    if (writep) {
        if (writeparam_list_.size() > MAXLISTSIZE) {
            FreeWriteParam(writep);
        } else {
            writeparam_list_.push_back(writep);
        }
    }
    while (true) {
        uv_mutex_lock(&mutex_writebuf_);
        if (write_circularbuf_.empty()) {
            uv_mutex_unlock(&mutex_writebuf_);
            break;
        }
        if (writeparam_list_.empty()) {
            writep = AllocWriteParam();
            writep->write_req_.data = this;
        } else {
            writep = writeparam_list_.front();
            writeparam_list_.pop_front();
        }
        writep->buf_.len = write_circularbuf_.read(writep->buf_.base, writep->buf_truelen_);
        uv_mutex_unlock(&mutex_writebuf_);
        int iret = uv_write((uv_write_t*)&writep->write_req_, (uv_stream_t*)&client_handle_->udphandle, &writep->buf_, 1, AfterSend);
        if (iret) {
            writeparam_list_.push_back(writep);//failure not call AfterSend. so recycle req
            LOGE("client(" << this << ") send error:" << GetUVError(iret));
            fprintf(stdout, "send error. %s-%s\n", uv_err_name(iret), uv_strerror(iret));
        }
    }
}

void UdpClient::AfterSend(uv_write_t* req, int status)
{
    UdpClient* theclass = (UdpClient*)req->data;
    if (status < 0) {
        if (theclass->writeparam_list_.size() > MAXLISTSIZE) {
            FreeWriteParam((write_param*)req);
        } else {
            theclass->writeparam_list_.push_back((write_param*)req);
        }
        LOGE("send error:" << GetUVError(status));
        fprintf(stderr, "send error %s\n", GetUVError(status));
        return;
    }
    theclass->send_inl(req);
}

void UdpClient::GetPacket(const unsigned char* packetdata, const unsigned packetlen, void* userdata)
{
    assert(userdata);
    UdpClientCtx* theclass = (UdpClientCtx*)userdata;
    UdpClient* parent = (UdpClient*)theclass->parent_server;
    if (parent->recvcb_) {//cb the data to user
        parent->recvcb_(packetdata,packetlen, parent->recvcb_userdata_);
    }
}

void UdpClient::AsyncCB(uv_async_t* handle)
{
    UdpClient* theclass = (UdpClient*)handle->data;
    if (theclass->isuseraskforclosed_) {
        theclass->closeinl();
        return;
    }
    //check data to send
    theclass->send_inl(NULL);
}

/* Fully close a loop */
void UdpClient::CloseWalkCB(uv_handle_t* handle, void* arg)
{
    UdpClient* theclass = (UdpClient*)arg;
    if (!uv_is_closing(handle)) {
        uv_close(handle, AfterClientClose);
    }
}

void UdpClient::setRecvCB(ClientRecvCB pfun, void* userdata)
{
    recvcb_ = pfun;
    recvcb_userdata_ = userdata;
}

void UdpClient::setClosedCB(TcpCloseCB pfun, void* userdata)
{
    closedcb_ = pfun;
    closedcb_userdata_ = userdata;
}

void UdpClient::StartLog(const char* logpath /*= nullptr*/)
{
    zsummer::log4z::ILog4zManager::getInstance()->setLoggerMonthdir(LOG4Z_MAIN_LOGGER_ID, true);
    zsummer::log4z::ILog4zManager::getInstance()->setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, false);
    zsummer::log4z::ILog4zManager::getInstance()->setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
    zsummer::log4z::ILog4zManager::getInstance()->setLoggerLimitsize(LOG4Z_MAIN_LOGGER_ID, 100);
    if (logpath) {
        zsummer::log4z::ILog4zManager::getInstance()->setLoggerPath(LOG4Z_MAIN_LOGGER_ID, logpath);
    }
    zsummer::log4z::ILog4zManager::getInstance()->start();
}

void UdpClient::StopLog()
{
    zsummer::log4z::ILog4zManager::getInstance()->stop();
}

}//end of namespace uv