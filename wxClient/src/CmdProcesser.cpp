#include "CmdProcesser.h"
#include "SocketData.h"
#include "CmdFactory.h"
#include "udp_msg.discover.pb.h"
#include "cmd_def.h"
#include "ProtobufUtils.h"

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

void CmdProcesser::protobuf_test(const char* msg_type_name,const char* buf,size_t len)
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

void CmdProcesser::test()
{
    cmd_message_t* buf = nullptr;
    unsigned int len;
    make_cmd(cmd_types::MESSAGE,(unsigned char**)&buf,&len,(const unsigned char*)"1234567",8);

    char line[100] = {0};
    memcpy(line,buf->payload,buf->header.length);
    short bt = 0;
    for(int i = 0; i < buf->header.length;i++){
        bt = buf->payload[i];
    }

    //check_buf_is_cmd((const unsigned char*)buf,len);

    free(buf);
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
