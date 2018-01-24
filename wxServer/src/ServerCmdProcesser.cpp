#include "ServerCmdProcesser.h"

ServerCmdProcesser::ServerCmdProcesser(wxEvtHandler* evtHandler,int clientId)
    : m_wxEvtHandler(evtHandler),m_clientId(clientId)
{
    m_isRecvingFilePartData = false;
    //m_cmdParser.setCmdBufParserCb(std::bind(&ServerCmdProcesser::onRecvTcpCmd,this,placeholders::_1,placeholders::_2));
}

void ServerCmdProcesser::onRecvCmd(const unsigned char* buf,const unsigned len)
{

}
