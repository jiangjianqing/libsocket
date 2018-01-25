#include "ClientCmdProcesser.h"
#include "CmdFactory.h"
#include "udp_msg.discover.pb.h"
#include "cmd_def.h"
#include "ProtobufUtils.h"
#include "CmdProcesserThreadEvent.h"
#include "tcp_msg.package.pb.h"
#include "tcp_msg.cmd.file.pb.h"
#include "ProtobufUtils.h"
#include "commonutils/FileUtils.h"


ClientCmdProcesser::ClientCmdProcesser(wxEvtHandler* evtHandler):m_wxEvtHandler(evtHandler),m_CurrFileNameListIndex(-1)
{
    m_luaEngine.testLua();
    //特别注意，该路径一定要是某个子目录，因为后续会对其进行删除清空
    m_recvFilePath = FileUtils::currentPath()+"/recv";
    //m_cmdParser.setCmdBufParserCb(std::bind(&ClientCmdProcesser::onRecvTcpCmd,this,placeholders::_1,placeholders::_2));
}

ClientCmdProcesser::~ClientCmdProcesser()
{
    clearFileBriefInfoList();
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

void ClientCmdProcesser::onRecvCmd(const unsigned char* buf,const unsigned len)
{
    //注意线程间通讯
    cmd_message_s* cmd = (cmd_message_s*)buf;
    tcp_msg::ProtoPackage pkg;
    if(pkg.ParseFromArray(cmd->payload,cmd->header.length)){//解析成功
        Message* msg = ProtobufUtils::createMessage(pkg.type_name());
        if(msg == nullptr){
            //未识别的指令类型
            assert("未识别的指令类型");
            return;
        }
        const string& strData = pkg.data();
        if(msg->ParseFromArray(strData.data(),strData.length())){
            string a = pkg.type_name();
            if(strcmp(a.c_str(),"tcp_msg.global.IdentifyRequest") == 0){
                callCmdEventCb(CmdEventType::TcpIdentifyResponse);
            }else if(strcmp(a.c_str(),"tcp_msg.global.StartUpdateRequest") == 0){
                callCmdEventCb(CmdEventType::TcpFileListRequest);
            }else if(dynamic_cast<tcp_msg::file::FileListResponse*>(msg) != nullptr){

                //清空指定目录，接收文件清单
                FileUtils::rm_rf(m_recvFilePath);
                tcp_msg::file::FileListResponse* filelistMsg= dynamic_cast<tcp_msg::file::FileListResponse*>(msg);

                clearFileBriefInfoList();
                for(int i = 0;i < filelistMsg->fileinfo_size();i++){
                    const tcp_msg::file::FileInfo& fileInfo = filelistMsg->fileinfo(i);
                    file_brief_info_t* briefInfo = CmdFactory::generateFileBriefInfoFromProtobufFileInfo(&fileInfo);
                    pushFileBriefInfoToList(briefInfo);
                }
                callCmdEventCb(CmdEventType::TcpSendFileRequest_NextFile);
            }else if(dynamic_cast<tcp_msg::file::SendFileResponse*>(msg) != nullptr){
                tcp_msg::file::SendFileResponse* fileRespMsg = dynamic_cast<tcp_msg::file::SendFileResponse*>(msg);

                switch(fileRespMsg->result()){
                case tcp_msg::file::SendFileResponse_SendFileResult_WHOLE:{
                    //将文件保存到指定目录
                    string filename = m_recvFilePath + fileRespMsg->filename();
                    const string& content = fileRespMsg->content();
                    string parentPath = FileUtils::parentPath(filename);
                    if(!FileUtils::exists(parentPath)){
                        FileUtils::mkdirp(parentPath);
                    }
                    FileUtils::saveDataAsFile(filename,content.data(),content.length());
                    callCmdEventCb(CmdEventType::TcpSendFileRequest_NextFile);
                    }//end of case
                    break;
                case tcp_msg::file::SendFileResponse_SendFileResult_PART:{//PART，将文件追加到已有文件中
                    string filename = m_recvFilePath + fileRespMsg->filename();
                    const string& content = fileRespMsg->content();
                    string parentPath = FileUtils::parentPath(filename);
                    if(!FileUtils::exists(parentPath)){
                        FileUtils::mkdirp(parentPath);
                    }
                    FileUtils::appendDataToFile(filename,content.data(),content.length());
                    }//end of case
                    break;
                default:
                    assert("未处理的tcp_msg::file::SendFileResponse_SendFileResult type");
                    break;
                //SendFileResponse_SendFileResult_UNKNOW_SEND_FILE_RESULT,
                //SendFileResponse_SendFileResult_NOTFOUND = 1,

                }

            }


        }else{
            //解析失败
            //assert("解析失败");
        }
        delete msg;
    }else{//解析失败

    }

    //CmdFactory::makeIdentifyResponseMsg(1,buf,len);
}

void ClientCmdProcesser::clearFileBriefInfoList()
{
    for(auto it = m_fileBriefInfoList.begin(); it != m_fileBriefInfoList.end();it++){
        file_brief_info_t* info = *it;
        free(info);
    }
    m_fileBriefInfoList.clear();
    m_CurrFileNameListIndex = -1;
}

void ClientCmdProcesser::pushFileBriefInfoToList(file_brief_info_t* fileBriefInfo)
{
    m_fileBriefInfoList.push_back(fileBriefInfo);
}

string ClientCmdProcesser::getCurrFilename()
{
    if(m_CurrFileNameListIndex < 0){
        return "";
    }
    if(m_CurrFileNameListIndex >= m_fileBriefInfoList.size()){
        return "";
    }
    file_brief_info_t* fileBriefInfo = m_fileBriefInfoList.at(m_CurrFileNameListIndex);
    return fileBriefInfo->filename;
}

bool ClientCmdProcesser::toNextFilename()
{
    //注意跨线程访问的问题
    int len = m_fileBriefInfoList.size();
    if(m_CurrFileNameListIndex < len){
        m_CurrFileNameListIndex++;
    }
    return m_CurrFileNameListIndex < len;
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
