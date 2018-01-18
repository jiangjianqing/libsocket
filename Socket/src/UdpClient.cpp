#include "UdpClient.h"
#include <thread>

UdpClient::UdpClient():AbstractSocket(SocketType::UDP)
{
    m_socketData = new SocketData(-1,this,(uv_handle_t*)malloc(sizeof(uv_udp_t)));
    //2018.01.11 重要： 为了沿用AbstractSocket中对loop的多线程处理，必须将忽略m_socketData中对于自身handle的处理
    m_socketData->setExternalHandle(handle());
    m_handleData.socketData = m_socketData;

    //m_broadcastSocketData = new SocketData(-1,this,(uv_handle_t*)malloc(sizeof(uv_udp_t)));
    //m_broadcastSocketData->setExternalHandle((uv_handle_t*)&m_handleBroadcast);
    //m_handleBroadcast.data = this;
}

UdpClient::~UdpClient()
{
    close();

    //注：m_socketData将由m_handleData负责释放
    //delete m_socketData;
    //delete m_broadcastSocketData;
}

void UdpClient::close()
{
    if(!m_isInited){
        return;
    }

    closeClient(m_socketData);
}

bool UdpClient::connect(int port)
{
    close();
    if(!init()){
        return false;
    }

    struct sockaddr_in recv_addr;
    int iret = uv_ip4_addr("0.0.0.0", port, &recv_addr);
    iret = uv_udp_bind((uv_udp_t*)handle(), (const struct sockaddr *)&recv_addr, UV_UDP_REUSEADDR);
    iret = uv_udp_recv_start((uv_udp_t*)handle(), onAllocBuffer, onAfterRead);

    m_port = port;

    run();
    return true;
}

void UdpClient::send(const uv_buf_t bufs[],unsigned int nbufs,const struct sockaddr* addr)
{
    uv_udp_send_t* req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    uv_udp_send(req, (uv_udp_t*)handle(), bufs, nbufs, addr, onAfterSend);
}

void UdpClient::send(const char* data,size_t len,const struct sockaddr* addr)
{
    uv_buf_t buf = uv_buf_init((char*)data, len);
    uv_udp_send_t* req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    uv_udp_send(req, (uv_udp_t*)handle(), &buf, 1, addr, onAfterSend);
}

void UdpClient::closeClientByThread(SocketData* cdata)
{
    thread t1(&UdpClient::closeClient,cdata);
    t1.detach();
}

void UdpClient::closeClient(SocketData* cdata)
{

    UdpClient *client = (UdpClient *)cdata->socket();
    if (uv_is_active((uv_handle_t*)cdata->handle())) {
        uv_read_stop((uv_stream_t*)cdata->handle());
    }

    uv_udp_recv_stop((uv_udp_t*)cdata->handle());
    //uv_close((uv_handle_t*)&client->m_handleBroadcast,nullptr);

    uv_close((uv_handle_t*)cdata->handle(),onAfterClientClose);

    //关闭连 = 关闭整个socket
    client->AbstractSocket::close();
}

void UdpClient::onAfterClientClose(uv_handle_t *handle)
{
    //fprintf(stdout,"客户端(%p)已close\n",handle);
    //LOGI("客户端("<<handle<<")已close");
}

void UdpClient::onAfterSend(uv_udp_send_t* req, int status)
{
    if(status){//重要：如果没有错误却调用getUVError，在valgrind中会出现内存泄露提示
        string err = getUVError(status);
        int i = 0;
        i = i + 1;
    }
    free(req);
}

void UdpClient::onAfterRead(uv_udp_t* handle
                            ,ssize_t nread,
                            const uv_buf_t* buf,
                            const struct sockaddr* addr,
                            unsigned flags)
{
    handle_data_t* handleData = static_cast<handle_data_t*>(handle->data);
    UdpClient* client = dynamic_cast<UdpClient*>(handleData->socket);
    if (nread < 0) {
        string err = getUVError(nread);
        FATAL(err);
        //fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        closeClientByThread(client->m_socketData);
        return;
    }else if(nread == 0){

    }else{

        socket_event_t event(socket_event_type::ReadData,handleData->socketData);
        auto reply = [&client,&addr](const char* buf,int len){
            client->send(buf,len,addr);
        };

        client->callSocketEventCb(event,buf->base,nread,reply);

        //echo code
        /*
        SocketData::reverse(buf->base,nread);
        //uv_buf_t newBuf = uv_buf_init(buf->base , nread);
        client->send(buf->base,nread,addr);
        //client->send(buf->base,nread,addr);
        */
    }

    //释放read缓存
    client->freeBuf(buf);
    /*
        char sender[17] = { 0 };
        uv_ip4_name((const struct sockaddr_in*) addr, sender, 16);
        fprintf(stderr, "Recv from %s\n", sender);

        // ... DHCP specific code
        unsigned int *as_integer = (unsigned int*)buf->base;
        unsigned int ipbin = ntohl(as_integer[4]);
        unsigned char ip[4] = {0};
        int i;
        for (i = 0; i < 4; i++)
            ip[i] = (ipbin >> i*8) & 0xff;
        fprintf(stderr, "Offered IP %d.%d.%d.%d\n", ip[3], ip[2], ip[1], ip[0]);

        free(buf->base);*/


}
/*
void UdpClient::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    if (!handle->data) {
        return;
    }
    UdpClient* client = static_cast<UdpClient*>(handle->data);
    SocketData *cdata = client->m_socketData;
    *buf = cdata->readbuffer;
}
*/
