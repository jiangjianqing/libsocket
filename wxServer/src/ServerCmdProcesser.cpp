#include "ServerCmdProcesser.h"
#include "tcp_msg.package.pb.h"
#include "tcp_msg.cmd.global.pb.h"
#include "tcp_msg.cmd.file.pb.h"
#include "CmdProcesserThreadEvent.h"
#include "ProtobufUtils.h"

ServerCmdProcesser::ServerCmdProcesser(wxEvtHandler* evtHandler,int clientId)
    : m_wxEvtHandler(evtHandler),AbstractServerCmdProcessor(clientId)
{

}

void ServerCmdProcesser::callCmdEventCb(const int& eventTypeId)
{

    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    //wxThreadEvent e;
    //wxThreadEvent e(wxEVT_COMMAND_THREAD, wxID_ANY);
    CmdProcesserThreadEvent event(this);
    event.SetId(eventTypeId);
    //event.SetId((int)socket_event_type::ConnectionAccept);
    //ostringstream ostr;
    //ostr<<ip<<":"<<port;
    //string info = std::to_string(port);
    //event.SetString(ostr.str());
    wxQueueEvent(m_wxEvtHandler,event.Clone());
    //wxTheApp->QueueEvent(e.Clone());
}
