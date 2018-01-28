#ifndef ABSTRACTSERVERCMDPROCESSER_H
#define ABSTRACTSERVERCMDPROCESSER_H

#include "CmdBufParser.h"
#include <vector>

enum class ServerCmdEventType{
    UNKNOW,TcpIdentifyResponse,TcpFileListResponse,TcpSendFileResponse,TcpAllFilesSendResult
};

using namespace std;

class AbstractServerCmdProcessor : public CmdBufParser
{
public:
    explicit AbstractServerCmdProcessor(int clientId);

    int clientId(){return m_clientId;}
    int identifyResponseId(){return m_identifyResponseId;}
    string currRequestFilename(){return m_currRequestFilename;}
    uint64_t currRequestFileStartPos(){return m_currStartPos;}
    vector<string> feedbackInfos(){return m_feedbackInfos;}
    bool isAllFilesSendOk(){return m_isAllFilesSendOk;}

protected:
    virtual void callCmdEventCb(const int& eventTypeId) = 0;
    void onRecvCmd(const unsigned char* buf,const unsigned len) override;
private:
    int m_clientId;//tcp通讯时使用的ID
    int m_identifyResponseId;//客户端发回的ID，用于Server根据ID来区分发送内容

    bool m_isRecvingFilePartData;
    bool m_isAllFilesSendOk;//是否FileList中的文件都已经发送成功

    vector<string> m_feedbackInfos;

    string m_currRequestFilename;
    uint64_t m_currStartPos;

    void resetFeedbackInfos(const vector<string>& feedbacks);

};

#endif // ABSTRACTSERVERCMDPROCESSER_H
