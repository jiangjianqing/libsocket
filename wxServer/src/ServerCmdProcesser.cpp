#include "ServerCmdProcesser.h"

ServerCmdProcesser::ServerCmdProcesser(wxEvtHandler* evtHandler) : m_wxEvtHandler(evtHandler)
{
    m_isRecvingFilePartData = false;
}

void ServerCmdProcesser::recvTcpData(const char* buf,int nread,socket_reply_cb replyCb)
{

}
