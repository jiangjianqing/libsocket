#include "ClientCmdProcesser.h"
#include "CmdFactory.h"
#include "udp_msg.discover.pb.h"
#include "cmd_def.h"
#include "ProtobufUtils.h"
#include "CmdProcesserThreadEvent.h"
#include "tcp_msg.package.pb.h"
#include "ProtobufUtils.h"

ClientCmdProcesser::ClientCmdProcesser(wxEvtHandler* evtHandler):m_wxEvtHandler(evtHandler)
{
    m_luaEngine.testLua();
    m_cmdParser.setCmdBufParserCb(std::bind(&ClientCmdProcesser::onRecvTcpCmd,this,placeholders::_1,placeholders::_2));
}

void ClientCmdProcesser::recvUdpData(const char* buf,int nread)
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

void ClientCmdProcesser::recvTcpData(const char* buf,int nread)
{
    if(m_cmdParser.isSingleEntireCmd((const unsigned char*)buf,nread)){
        m_cmdParser.cleanParserBuf();
        onRecvTcpCmd((const unsigned char*)buf,nread);
        //直接处理
    }else{
        m_cmdParser.inputData((const unsigned char*)buf,nread);
    }

}

void ClientCmdProcesser::onRecvTcpCmd(const unsigned char* buf,const unsigned len)
{
    //注意线程间通讯
    cmd_message_s* cmd = (cmd_message_s*)buf;
    tcp_msg::ProtoPackage pkg;
    if(pkg.ParseFromArray(cmd->payload,cmd->header.length)){//解析成功
        Message* msg = ProtobufUtils::createMessage(pkg.type_name());
        if(msg != nullptr){
            string a = pkg.type_name();
            if(strcmp(a.c_str(),"tcp_msg.IdentifyRequest") == 0){
                int i = 0;
                i = i + 1;
            }

            delete msg;
        }

    }else{//解析失败

    }

    //CmdFactory::makeIdentifyResponseMsg(1,buf,len);
}

void ClientCmdProcesser::protobuf_test(const char* msg_type_name,const char* buf,size_t len)
{
    //string msg_type_name = "udp_msg.discover";//以package的命名方式
    const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(msg_type_name);
    const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
    Message* msg = prototype->New();
    if(msg->ParseFromArray(buf,len)){
        int i = 0;
        i = i + 1;
    }
    //assert(prototype == &T::default_instance());
    cout << "GetPrototype()        = " << prototype << endl;
    //cout << "T::default_instance() = " << &T::default_instance() << endl;
    cout << endl;
    //assert(descriptor == T::descriptor());
    cout << "FindMessageTypeByName() = " << descriptor << endl;
    //cout << "T::descriptor()         = " << T::descriptor() << endl;
    cout << endl;

    delete msg;
}

void ClientCmdProcesser::test()
{

    cmd_message_t* buf = nullptr;
    unsigned int len;
    CmdBufParser::makeCmd(cmd_types::MESSAGE,(unsigned char**)&buf,&len,(const unsigned char*)"1234567",8);

    char line[100] = {0};
    memcpy(line,buf->payload,buf->header.length);
    short bt = 0;
    for(int i = 0; i < buf->header.length;i++){
        bt = buf->payload[i];
    }

    //check_buf_is_cmd((const unsigned char*)buf,len);

    free(buf);

}

void ClientCmdProcesser::callCmdEventCb(const CmdEventType& eventType)
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
