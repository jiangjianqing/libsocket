#include "ClientCmdProcessor.h"
#include "CmdFactory.h"
#include "udp_msg.discover.pb.h"
#include "cmd_def.h"
#include "ProtobufUtils.h"
#include "CmdProcesserThreadEvent.h"
#include "tcp_msg.package.pb.h"
#include "tcp_msg.cmd.file.pb.h"
#include "tcp_msg.cmd.task.pb.h"
#include "ProtobufUtils.h"


ClientCmdProcessor::ClientCmdProcessor(wxEvtHandler* evtHandler)
    :m_wxEvtHandler(evtHandler)
{
    m_luaEngine.testLua();
    //当前使用父类默认的recvFilePath参数
    //m_recvFilePath = FileUtils::currentPath()+"/recv";

    m_luaEngine.setSourceDir(m_recvFilePath);
    m_luaEngine.loadParamFromLuaScript();

    setIdentifyResponseId(m_luaEngine.identifyResponseId());
    //m_cmdParser.setCmdBufParserCb(std::bind(&ClientCmdProcesser::onRecvTcpCmd,this,placeholders::_1,placeholders::_2));
}

ClientCmdProcessor::~ClientCmdProcessor()
{

}

void ClientCmdProcessor::callCmdEventCb(const int& eventTypeId)
{

    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    //wxThreadEvent e;
    //wxThreadEvent e(wxEVT_COMMAND_THREAD, wxID_ANY);
    CmdProcesserThreadEvent event(wxEVT_COMMAND_THREAD);
    event.SetId(eventTypeId);
    //event.SetId((int)socket_event_type::ConnectionAccept);
    //ostringstream ostr;
    //ostr<<ip<<":"<<port;
    //string info = std::to_string(port);
    //event.SetString(ostr.str());
    wxQueueEvent(m_wxEvtHandler,event.Clone());

    if(eventTypeId == (int)ClientCmdEventType::TcpExecuteTaskRequest ){
        m_luaEngine.executeTask(currTaskname());
    }
    //wxTheApp->QueueEvent(e.Clone());
}

//无法解析buf为ProtoPackage时会调用，由sub class处理
void ClientCmdProcessor::onRecvUnknowMessage(const unsigned char* buf,const unsigned len)
{

}

//无法根据type_name创建protobuf时调用
void ClientCmdProcessor::onRecvUnknowProtoPackage(const unsigned char* buf,const unsigned len)
{

}

void ClientCmdProcessor::onRecvErrorProtoPackage(const unsigned char* buf,const unsigned len)
{

}
