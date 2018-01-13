#include "UdpServer.h"
#include <thread>

UdpServer::UdpServer():AbstractSocket(SocketType::UDP)
{
    m_socketData = new SocketData(-1,this,(uv_handle_t*)malloc(sizeof(uv_udp_t)));
    //2018.01.11 重要： 为了沿用AbstractSocket中对loop的多线程处理，必须将忽略m_socketData中对于自身handle的处理
    m_socketData->setExternalHandle(handle());
}

UdpServer::~UdpServer()
{
    close();

    delete m_socketData;
}

void UdpServer::close()
{
    if(!m_isInited){
        return;
    }

    closeClient(m_socketData);
}

bool UdpServer::start(int port)
{
    close();
    if(!init()){
        return false;
    }

    struct sockaddr_in recv_addr;
    int iret = uv_ip4_addr("0.0.0.0", port, &recv_addr);
    iret = uv_udp_bind((uv_udp_t*)handle(), (const struct sockaddr *)&recv_addr, UV_UDP_REUSEADDR);
    //iret = uv_udp_recv_start((uv_udp_t*)handle(), onAllocBuffer, onAfterRead);

    //The IP address 0.0.0.0 is used to bind to all interfaces.
    //The IP address 255.255.255.255 is a broadcast address meaning that packets will be sent to all interfaces on the subnet.
    //port 0 means that the OS randomly assigns a port.

    iret = uv_udp_init(m_loop, &m_sendHandle);
    struct sockaddr_in broadcast_addr;
    iret = uv_ip4_addr("0.0.0.0", 0, &broadcast_addr);
    iret = uv_udp_bind(&m_sendHandle, (const struct sockaddr *)&broadcast_addr, 0);
    iret = uv_udp_set_broadcast(&m_sendHandle, 1);

    m_port = port;

    run();
    return true;
}

void UdpServer::broadcast(const char* data,size_t len)
{
    //uv_buf_t* buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));//uv_buf_init((char*)data, len);//make_discover_msg();
    uv_buf_t buf = uv_buf_init((char*)data, len);
    //The IP address 0.0.0.0 is used to bind to all interfaces.
    //The IP address 255.255.255.255 is a broadcast address meaning that packets will be sent to all interfaces on the subnet.
    //port 0 means that the OS randomly assigns a port.
    struct sockaddr_in send_addr;
    uv_ip4_addr("255.255.255.255", m_port, &send_addr);
    uv_udp_send_t* req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    uv_udp_send(req, &m_sendHandle, &buf, 1, (const struct sockaddr *)&send_addr, onAfterSend);
}

void UdpServer::send(const char* data,size_t len,const struct sockaddr* addr)
{
    uv_buf_t buf = uv_buf_init((char*)data, len);
    uv_udp_send_t* req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    uv_udp_send(req, (uv_udp_t*)handle(), &buf, 1, addr, onAfterSend);
}

void UdpServer::closeClientByThread(SocketData* cdata)
{
    thread t1(&UdpServer::closeClient,cdata);
    t1.detach();
}

void UdpServer::closeClient(SocketData* cdata)
{

    UdpServer *client = (UdpServer *)cdata->socket();
    if (uv_is_active((uv_handle_t*)cdata->handle())) {
        uv_read_stop((uv_stream_t*)cdata->handle());
    }

    uv_udp_recv_stop((uv_udp_t*)cdata->handle());
    uv_close((uv_handle_t*)&client->m_sendHandle,nullptr);

    uv_close((uv_handle_t*)cdata->handle(),onAfterClientClose);

    //关闭连 = 关闭整个socket
    client->AbstractSocket::close();
}

void UdpServer::onAfterClientClose(uv_handle_t *handle)
{
    //fprintf(stdout,"客户端(%p)已close\n",handle);
    //LOGI("客户端("<<handle<<")已close");
}

void UdpServer::onAfterSend(uv_udp_send_t* req, int status)
{
    string err = getUVError(status);
    free(req);
}

void UdpServer::onAfterRead(uv_udp_t* handle
                            ,ssize_t nread,
                            const uv_buf_t* buf,
                            const struct sockaddr* addr,
                            unsigned flags)
{
    UdpServer* client = static_cast<UdpServer*>(handle->data);
    if (nread < 0) {
        //fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        closeClientByThread(client->m_socketData);
        return;
    }else if(nread == 0){

    }else{
        SocketData::reverse(buf->base,nread);
        //uv_buf_t newBuf = uv_buf_init(buf->base , nread);
        client->send(buf->base,nread,addr);
        //client->send(buf->base,nread,addr);
    }
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

void UdpServer::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    if (!handle->data) {
        return;
    }
    UdpServer* client = static_cast<UdpServer*>(handle->data);
    SocketData *cdata = client->m_socketData;
    *buf = cdata->readbuffer;
}