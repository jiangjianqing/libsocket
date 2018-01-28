#include "AbstractClientCmdProcessor.h"
#include "CmdFactory.h"
#include "protos/udp_msg.discover.pb.h"
#include "cmd_def.h"
#include "ProtobufUtils.h"
#include "protos/tcp_msg.package.pb.h"
#include "protos/tcp_msg.cmd.file.pb.h"
#include "protos/tcp_msg.cmd.task.pb.h"
#include "ProtobufUtils.h"
#include "commonutils/FileUtils.h"


AbstractClientCmdProcessor::AbstractClientCmdProcessor()
    :m_CurrFileNameListIndex(-1),m_identifyResponseId(-1)
{
    //特别注意，该路径一定要是某个子目录，因为后续会对其进行删除清空
    m_recvFilePath = FileUtils::currentPath()+"/recv";
}

AbstractClientCmdProcessor::~AbstractClientCmdProcessor()
{
    clearFileBriefInfoList();
}

void AbstractClientCmdProcessor::recvUdpData(const char* buf,int nread)
{
    //只识别一个discover命令,而且为可见字符集
    udp_msg::discover msg;
    if(msg.ParseFromArray(buf,nread)){
        //收到discover信息
        m_serverIp = msg.server_ip();
        m_serverPort = msg.server_port();

        callCmdEventCb((int)ClientCmdEventType::UdpDiscover);
    }
}

void AbstractClientCmdProcessor::onRecvCmd(const unsigned char* buf,const unsigned len)
{
    //注意线程间通讯
    cmd_message_t* cmd = (cmd_message_t*)buf;
    tcp_msg::ProtoPackage pkg;
    if(pkg.ParseFromArray(cmd->payload,cmd->header.length)){//解析成功
        Message* msg = ProtobufUtils::createMessage(pkg.type_name());
        if(msg == nullptr){
            //未识别的指令类型
            onRecvUnknowProtoPackage(cmd->payload,cmd->header.length);
            return;
        }
        const string& strData = pkg.data();
        if(msg->ParseFromArray(strData.data(),strData.length())){
            string a = pkg.type_name();
            if(strcmp(a.c_str(),"tcp_msg.global.IdentifyRequest") == 0){
                callCmdEventCb((int)ClientCmdEventType::TcpIdentifyResponse);
            }else if(strcmp(a.c_str(),"tcp_msg.global.StartUpdateRequest") == 0){
                callCmdEventCb((int)ClientCmdEventType::TcpFileListRequest);
            }else if(dynamic_cast<::tcp_msg::task::ExecuteTaskRequest*>(msg) != nullptr){
                ::tcp_msg::task::ExecuteTaskRequest* taskRequestMsg = dynamic_cast<::tcp_msg::task::ExecuteTaskRequest*>(msg);
                const string& taskname = taskRequestMsg->taskname();
                m_currTaskname = taskname;
                callCmdEventCb((int)ClientCmdEventType::TcpExecuteTaskRequest);
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
                callCmdEventCb((int)ClientCmdEventType::TcpSendFileRequest_NextFile);
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
                    callCmdEventCb((int)ClientCmdEventType::TcpSendFileRequest_NextFile);
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
                    if(fileRespMsg->residue_length() == 0){//文件全部收完
                        callCmdEventCb((int)ClientCmdEventType::TcpSendFileRequest_NextFile);
                    }else{//取文件的下一个part
                        callCmdEventCb((int)ClientCmdEventType::TcpSendFileRequest_CurrentFile);
                    }
                    }//end of case
                    break;
                default:
                    assert("未处理的tcp_msg::file::SendFileResponse_SendFileResult type");
                    break;
                //SendFileResponse_SendFileResult_UNKNOW_SEND_FILE_RESULT,
                //SendFileResponse_SendFileResult_NOTFOUND = 1,

                }

            }


        }else{//end of if(msg->ParseFromArray(...
            //解析失败
            onRecvErrorProtoPackage(buf,len);
        }
        delete msg;
    }else{//end of if(pkg.ParseFromArray(...
        //解析失败
        onRecvUnknowMessage(buf,len);
    }

    //CmdFactory::makeIdentifyResponseMsg(1,buf,len);
}

void AbstractClientCmdProcessor::clearFileBriefInfoList()
{
    for(auto it = m_fileBriefInfoList.begin(); it != m_fileBriefInfoList.end();it++){
        file_brief_info_t* info = *it;
        free(info);
    }
    m_fileBriefInfoList.clear();
    m_CurrFileNameListIndex = -1;
}

void AbstractClientCmdProcessor::pushFileBriefInfoToList(file_brief_info_t* fileBriefInfo)
{
    m_fileBriefInfoList.push_back(fileBriefInfo);
}

string AbstractClientCmdProcessor::getCurrFilename()
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

int64_t AbstractClientCmdProcessor::getCurrFileStartPos()
{
    if(m_CurrFileNameListIndex < 0){
        return -1;
    }
    if(m_CurrFileNameListIndex >= m_fileBriefInfoList.size()){
        return -2;
    }
    file_brief_info_t* fileBriefInfo = m_fileBriefInfoList.at(m_CurrFileNameListIndex);
#ifdef __GNUC__
    //vc编译器不支持 常量表达式。。。。。。。
    char buf[strlen(fileBriefInfo->filename)+1] = {0};
    strcpy(buf,fileBriefInfo->filename);
#else
    string buf(fileBriefInfo->filename);
#endif
    string fullpath = m_recvFilePath + buf;
    return FileUtils::getFileSize(fullpath);
}

bool AbstractClientCmdProcessor::toNextFilename()
{
    //注意跨线程访问的问题
    int len = m_fileBriefInfoList.size();
    if(m_CurrFileNameListIndex < len){
        m_CurrFileNameListIndex++;
    }
    return m_CurrFileNameListIndex < len;
}
