#ifndef CMDPROCESSER_H
#define CMDPROCESSER_H

#include "SocketData.h"

class CmdProcesser
{
public:
    CmdProcesser();

    void recvUdpData(const char* buf,int nread,socket_reply_cb replyCb);
};

#endif // CMDPROCESSER_H
