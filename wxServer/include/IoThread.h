#ifndef IOTHREAD_H
#define IOTHREAD_H

//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include "uv.h"
//#include <QObject>
//#include "Type.h"
//#include "List.h"
#include <string>
using namespace std;
#define MAX_UDP_DATA_SIZE 65507

typedef enum{
    etPostUdp
}IoRequestType;

typedef struct{
    //DoubleNode node;
    IoRequestType type;
    char *pData;
    unsigned nBytes;
}IoRequest;


class IoThread
{
public:
    IoThread();
    ~IoThread();
    void SetLocal(unsigned short nPort);
    void SetRemote(const string &strIp, unsigned short nPort);

    bool Run();

    void stop();

    bool PostUdp(char *pData, unsigned nBytes);

private:
    bool Post(IoRequest *pRequest);
    inline void ProcessRequests();
    inline void ProcessRequest(IoRequest *pRequest);
    static void onRequest(uv_async_t* handle);

    uv_thread_t _thread;
    uv_mutex_t _mutex;
    uv_async_t _uvaRequestNofity;
    uv_udp_t _udp;
//    uv_udp_send_t _udpSendRequest;
    struct sockaddr_in _siRemote;
};

#endif // IOTHREAD_H
