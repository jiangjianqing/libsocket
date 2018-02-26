#include "ServerCmdProcessor.h"
#include "tcp_msg.package.pb.h"
#include "tcp_msg.cmd.global.pb.h"
#include "tcp_msg.cmd.file.pb.h"
#include "CmdProcessorThreadEvent.h"
#include "ProtobufUtils.h"

ServerCmdProcessor::ServerCmdProcessor(wxEvtHandler* evtHandler,int clientId)
    : m_wxEvtHandler(evtHandler),AbstractServerCmdProcessor(clientId)
{

}

void ServerCmdProcessor::callCmdEventCb(const int& eventTypeId)
{

    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    //wxThreadEvent e;
    //wxThreadEvent e(wxEVT_COMMAND_THREAD, wxID_ANY);
    CmdProcessorThreadEvent event(this);
    event.SetId(eventTypeId);
    //event.SetId((int)socket_event_type::ConnectionAccept);
    //ostringstream ostr;
    //ostr<<ip<<":"<<port;
    //string info = std::to_string(port);
    //event.SetString(ostr.str());
    wxQueueEvent(m_wxEvtHandler,event.Clone());
    //wxTheApp->QueueEvent(e.Clone());
}
