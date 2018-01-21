#ifndef SERVERCMDPROCESSER_H
#define SERVERCMDPROCESSER_H

#include "wx/event.h"
#include "SocketData.h"

class ServerCmdProcesser
{
public:
    explicit ServerCmdProcesser(wxEvtHandler* evtHandler);

    void recvTcpData(const char* buf,int nread,socket_reply_cb replyCb);
private:
    wxEvtHandler* m_wxEvtHandler;
};

#endif // SERVERCMDPROCESSER_H
