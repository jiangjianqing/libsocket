#include "ServerCmdProcesser.h"
#include "tcp_msg.package.pb.h"
#include "tcp_msg.cmd.global.pb.h"
#include "tcp_msg.cmd.file.pb.h"
#include "CmdProcesserThreadEvent.h"
#include "ProtobufUtils.h"

ServerCmdProcesser::ServerCmdProcesser(wxEvtHandler* evtHandler,int clientId)
    : m_wxEvtHandler(evtHandler),m_clientId(clientId),m_identifyResponseId(-1)
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
        if(msg == nullptr){
            assert("无法识别的消息类型");
            return;
        }
        const string& str = pkg.data();
        string typeName = pkg.type_name();
        if(msg->ParseFromArray(str.data(),str.length()))
        {
            if(strcmp(typeName.c_str(),"tcp_msg.global.IdentifyResponse") == 0){

                if(msg->ParseFromArray(str.data(),str.length()))
                {
                    tcp_msg::global::IdentifyResponse* idMsg= dynamic_cast<tcp_msg::global::IdentifyResponse*>(msg);
                    m_identifyResponseId = idMsg->id();
                    callCmdEventCb(CmdEventType::TcpIdentifyResponse);
                }
            }else if(dynamic_cast<tcp_msg::file::FileListRequest*>(msg) != nullptr){
                callCmdEventCb(CmdEventType::TcpFileListResponse);
            }else if(dynamic_cast<tcp_msg::file::SendFileRequest*>(msg) != nullptr){
                tcp_msg::file::SendFileRequest* fileRequestMsg = dynamic_cast<tcp_msg::file::SendFileRequest*>(msg);
                m_currRequestFilename = fileRequestMsg->filename();
                m_currStartPos =fileRequestMsg->start_pos();
                //从这里开始取指定文件大小
                callCmdEventCb(CmdEventType::TcpSendFileResponse);
            }
        }
        delete msg;

    }else{//解析失败

    }
}

void ServerCmdProcesser::callCmdEventCb(const CmdEventType& eventType)
{

    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    //wxThreadEvent e;
    //wxThreadEvent e(wxEVT_COMMAND_THREAD, wxID_ANY);
    CmdProcesserThreadEvent event(this);
    event.SetId((int)eventType);
    //event.SetId((int)socket_event_type::ConnectionAccept);
    //ostringstream ostr;
    //ostr<<ip<<":"<<port;
    //string info = std::to_string(port);
    //event.SetString(ostr.str());
    wxQueueEvent(m_wxEvtHandler,event.Clone());
    //wxTheApp->QueueEvent(e.Clone());
}
