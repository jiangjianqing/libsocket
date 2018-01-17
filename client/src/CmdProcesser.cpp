#include "CmdProcesser.h"
#include "SocketData.h"
#include "CmdFactory.h"
#include "protos/udp_msg.discover.pb.h"

CmdProcesser::CmdProcesser(wxEvtHandler* evtHandler):m_wxEvtHandler(evtHandler)
{
    m_luaEngine.testLua();
}

void CmdProcesser::recvUdpData(const char* buf,int nread,socket_reply_cb replyCb)
{
    //只识别一个discover命令,而且为可见字符集
    udp_msg::discover msg;
    if(msg.ParseFromArray(buf,nread)){
        //收到discover信息
        m_serverIp = msg.server_ip();
        m_serverPort = msg.server_port();

        callCmdEventCb(CmdEventType::UdpDiscover);
    }
}

void CmdProcesser::callCmdEventCb(const CmdEventType& eventType)
{

    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    //wxThreadEvent e;
    wxThreadEvent e(wxEVT_COMMAND_THREAD, wxID_ANY);
    e.SetId((int)eventType);
    //event.SetId((int)socket_event_type::ConnectionAccept);
    //ostringstream ostr;
    //ostr<<ip<<":"<<port;
    //string info = std::to_string(port);
    //event.SetString(ostr.str());
    wxQueueEvent(m_wxEvtHandler,e.Clone());
    //wxTheApp->QueueEvent(e.Clone());
}
