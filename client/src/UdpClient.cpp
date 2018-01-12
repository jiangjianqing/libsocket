#include "UdpClient.h"
#include <thread>

UdpClient::UdpClient()
{
    m_socketData = new SocketData(-1,this,(uv_handle_t*)malloc(sizeof(uv_udp_t)));
    //2018.01.11 重要： 为了沿用AbstractSocket中对loop的多线程处理，必须将忽略m_socketData中对于自身handle的处理
    m_socketData->setExternalHandle(handle());
}

UdpClient::~UdpClient()
{
    close();

    delete m_socketData;
}

void UdpClient::close()
{
    if(!m_isInited){
        return;
    }

    closeClient(m_socketData);
}

void UdpClient::connect(int port)
{
    struct sockaddr_in recv_addr;
    uv_ip4_addr("0.0.0.0", port, &recv_addr);
    uv_udp_bind((uv_udp_t*)handle(), (const struct sockaddr *)&recv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start((uv_udp_t*)handle(), onAllocBuffer, onAfterRead);
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

    uv_close((uv_handle_t*)cdata->handle(),onAfterClientClose);

    //关闭连 = 关闭整个socket
    client->AbstractSocket::close();
}

void UdpClient::onAfterClientClose(uv_handle_t *handle)
{
    //fprintf(stdout,"客户端(%p)已close\n",handle);
    //LOGI("客户端("<<handle<<")已close");
}

void UdpClient::onAfterRead(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags)
{

}

void UdpClient::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    if (!handle->data) {
        return;
    }
    UdpClient* client = static_cast<UdpClient*>(handle->data);
    SocketData *cdata = client->m_socketData;
    *buf = cdata->readbuffer;
}
