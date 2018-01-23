#ifndef SERVERCMDPROCESSER_H
#define SERVERCMDPROCESSER_H

#include "wx/event.h"

class ServerCmdProcesser
{
public:
    explicit ServerCmdProcesser(wxEvtHandler* evtHandler , int clientId);

    void recvTcpData(const char* buf,int nread);
private:
    wxEvtHandler* m_wxEvtHandler;
    int m_clientId;

    bool m_isRecvingFilePartData;
};

#endif // SERVERCMDPROCESSER_H
