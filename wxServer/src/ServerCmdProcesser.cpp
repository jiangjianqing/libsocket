#include "ServerCmdProcesser.h"
#include "tcp_msg.package.pb.h"
#include "tcp_msg.cmd.pb.h"
#include "ProtobufUtils.h"

ServerCmdProcesser::ServerCmdProcesser(wxEvtHandler* evtHandler,int clientId)
    : m_wxEvtHandler(evtHandler),m_clientId(clientId)
{
    m_isRecvingFilePartData = false;
    //m_cmdParser.setCmdBufParserCb(std::bind(&ServerCmdProcesser::onRecvTcpCmd,this,placeholders::_1,placeholders::_2));
}

void ServerCmdProcesser::onRecvCmd(const unsigned char* buf,const unsigned len)
{
    //注意线程间通讯
    cmd_message_s* cmd = (cmd_message_s*)buf;
    tcp_msg::ProtoPackage pkg;
    if(pkg.ParseFromArray(cmd->payload,cmd->header.length)){//解析成功
        Message* msg = ProtobufUtils::createMessage(pkg.type_name());
        if(msg != nullptr){
            string a = pkg.type_name();
            if(strcmp(a.c_str(),"tcp_msg.IdentifyResponse") == 0){

                callCmdEventCb(CmdEventType::RecvTcpIdentifyResponse);
            }

            delete msg;
        }

    }else{//解析失败

    }
}

void ServerCmdProcesser::callCmdEventCb(const CmdEventType& eventType)
{

    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    //wxThreadEvent e;
    //wxThreadEvent e(wxEVT_COMMAND_THREAD, wxID_ANY);
    CmdProcesserThreadEvent event(wxEVT_COMMAND_THREAD);
    event.SetId((int)eventType);
    //event.SetId((int)socket_event_type::ConnectionAccept);
    //ostringstream ostr;
    //ostr<<ip<<":"<<port;
    //string info = std::to_string(port);
    //event.SetString(ostr.str());
    wxQueueEvent(m_wxEvtHandler,event.Clone());
    //wxTheApp->QueueEvent(e.Clone());
}
