#include "SocketData.h"
#include <stdlib.h>
#include <string.h> //memcmp在这里面声明

#define BUFFERSIZE (1024*1024)

SocketData::SocketData(int clientId,AbstractSocket* socket)
    : m_clientId(clientId),m_socket(socket),recvcb_(nullptr)
{
    //client_handle = (uv_tcp_t*)malloc(sizeof(*client_handle));
    m_socketHandle = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    m_socketHandle->data = this;
    readbuffer = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
    writebuffer = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);


}

SocketData::~SocketData()
{
    free(readbuffer.base);
    readbuffer.base = nullptr;
    readbuffer.len = 0;

    free(writebuffer.base);
    writebuffer.base = nullptr;
    writebuffer.len = 0;

    free(m_socketHandle);
    m_socketHandle = nullptr;
}

void SocketData::refreshInfo()
{
    //有连接，可以获得目标的ip和端口
    struct sockaddr sockname, peername;
    int namelen;
    int r;
    namelen = sizeof(peername);
    memset(&peername, -1, namelen);
    r = uv_tcp_getpeername(handle(), &peername, &namelen);

    m_ip = string(inet_ntoa(((sockaddr_in *)&peername)->sin_addr));
    m_port = ntohs(((sockaddr_in *)&peername)->sin_port);

}

void check_sockname(struct sockaddr* addr, const char* compare_ip,
        int compare_port, const char* context)
{
    struct sockaddr_in check_addr = *(struct sockaddr_in*) addr;

    char check_ip[17];
    int r;

    struct sockaddr_in compare_addr;
    uv_ip4_addr(compare_ip, compare_port,&compare_addr);

    /* Both addresses should be ipv4 */
    if (check_addr.sin_family == AF_INET) {
        printf("src sin_family is AF_INET.\n");
    }

    if (compare_addr.sin_family == AF_INET) {
        printf("compare sin_family is AF_INET.\n");
    }

    /* Check if the ip matches */
    if (memcmp(&check_addr.sin_addr, &compare_addr.sin_addr,
            sizeof compare_addr.sin_addr) == 0) {
        printf("ip matches.\n");
    }

    /* Check if the port matches. If port == 0 anything goes. */
    if (compare_port == 0 || check_addr.sin_port == compare_addr.sin_port) {
        printf("port matches.\n");
    }
    //网络字节序转换成主机字符序
    uv_ip4_name(&check_addr, (char*)check_ip, sizeof check_ip);

    //或者像下面这样获得ip地址
    //char* check_ip = inet_ntoa(check_addr.sin_addr);

    printf("%s: %s:%d\n", context, check_ip, ntohs(check_addr.sin_port));
}

bool SocketData::send(const char* data, std::size_t len)
{
    if (writebuffer.len < len) {
        writebuffer.base = (char*)realloc(writebuffer.base,len);
        writebuffer.len = len;
    }
    memcpy(writebuffer.base,data,len);
    uv_buf_t buf = uv_buf_init((char*)writebuffer.base,len);
    int iret = uv_write(&write_req, (uv_stream_t*)handle(), &buf, 1, onAfterSend);
    if (iret) {
           //errmsg = getUVError(iret);
           //LOGE(errmsg_);
           return false;
    }
    return true;
}

//服务器与客户端一致

void SocketData::onAfterSend(uv_write_t *req, int status)
{
    if (status < 0) {
        //m_errmsg = "发送数据有误:"<<getUVError(status);
            //LOGE("发送数据有误:"<<GetUVError(status));

            //fprintf(stderr, "Write error %s\n", GetUVError(status));
    }

}

//用于echo
void SocketData::reverse(char *s, int len)
{
    char c;
    char *end;
    end = s + len - 1;
    while (*end == '\r' || *end == '\n') {
        end--;
    }
    while (end > s) {
        c = *end;
        *end = *s;
        *s = c;
        end--;
        s++;
    }
}
