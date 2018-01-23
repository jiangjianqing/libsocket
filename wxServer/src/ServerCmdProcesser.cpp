#include "ServerCmdProcesser.h"

ServerCmdProcesser::ServerCmdProcesser(wxEvtHandler* evtHandler,int clientId)
    : m_wxEvtHandler(evtHandler),m_clientId(clientId)
{
    m_isRecvingFilePartData = false;
}

void ServerCmdProcesser::recvTcpData(const char* buf,int nread)
{
    int i = 0 ;
    i = i + 1;
}
