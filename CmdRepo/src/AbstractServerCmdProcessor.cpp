#include "AbstractServerCmdProcessor.h"
#include "protos/tcp_msg.package.pb.h"
#include "protos/tcp_msg.cmd.global.pb.h"
#include "protos/tcp_msg.cmd.file.pb.h"
#include "ProtobufUtils.h"

AbstractServerCmdProcessor::AbstractServerCmdProcessor(int clientId)
    : m_clientId(clientId),m_identifyResponseId(-1)
{
    m_isRecvingFilePartData = false;
    //m_cmdParser.setCmdBufParserCb(std::bind(&ServerCmdProcesser::onRecvTcpCmd,this,placeholders::_1,placeholders::_2));
}

void AbstractServerCmdProcessor::onRecvCmd(const unsigned char* buf,const unsigned len)
{
    //注意线程间通讯
    cmd_message_s* cmd = (cmd_message_s*)buf;
    tcp_msg::ProtoPackage pkg;
    if(pkg.ParseFromArray(cmd->payload,cmd->header.length)){//解析成功
        Message* msg = ProtobufUtils::createMessage(pkg.type_name());
        if(msg == nullptr){
            //assert("无法识别的消息类型");
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
                    callCmdEventCb((int)ServerCmdEventType::TcpIdentifyResponse);
                }
            }else if(dynamic_cast<tcp_msg::file::FileListRequest*>(msg) != nullptr){
                callCmdEventCb((int)ServerCmdEventType::TcpFileListResponse);
            }else if(dynamic_cast<tcp_msg::file::SendFileRequest*>(msg) != nullptr){
                tcp_msg::file::SendFileRequest* fileRequestMsg = dynamic_cast<tcp_msg::file::SendFileRequest*>(msg);
                m_currRequestFilename = fileRequestMsg->filename();
                m_currStartPos =fileRequestMsg->start_pos();
                //从这里开始取指定文件大小
                callCmdEventCb((int)ServerCmdEventType::TcpSendFileResponse);
            }else if(dynamic_cast<::tcp_msg::file::FileListSendResultResponse*>(msg) != nullptr){
                ::tcp_msg::file::FileListSendResultResponse* sendResultMsg = dynamic_cast<::tcp_msg::file::FileListSendResultResponse*>(msg);
                m_isAllFilesSendOk = sendResultMsg->result_type() == ::tcp_msg::SUCCESS;
                vector<string> feedbacks;
                for(int i = 0;i < sendResultMsg->result_info_size();i++){
                    const ::tcp_msg::FeedbackInfo& fb = sendResultMsg->result_info(i);
                    feedbacks.push_back(fb.info());
                }
                resetFeedbackInfos(feedbacks);
                callCmdEventCb((int)ServerCmdEventType::TcpAllFilesSendResult);
            }
        }
        delete msg;

    }else{//解析失败

    }
}

void AbstractServerCmdProcessor::resetFeedbackInfos(const vector<string>& feedbacks)
{
    m_feedbackInfos.clear();
    m_feedbackInfos.insert(m_feedbackInfos.end(),feedbacks.begin(),feedbacks.end());
}
