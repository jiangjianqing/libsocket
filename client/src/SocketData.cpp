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

    string ip(inet_ntoa(((sockaddr_in *)&peername)->sin_addr));
    int port = ntohs(((sockaddr_in *)&peername)->sin_port);
    int i = 0;
    i = i + 1;
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
