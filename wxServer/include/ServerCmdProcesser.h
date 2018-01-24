#ifndef SERVERCMDPROCESSER_H
#define SERVERCMDPROCESSER_H

#include "wx/event.h"
#include "CmdBufParser.h"

enum class CmdEventType{
    UNKNOW,TcpIdentifyResponse,TcpFileListResponse,TcpSendFileResponse
};

#define ID_CMDPROCESSER_THREADEVENT   wxID_HIGHEST+30

typedef function<void (const CmdEventType& event)> CmdEventCb;

class ServerCmdProcesser : public CmdBufParser
{
public:
    explicit ServerCmdProcesser(wxEvtHandler* evtHandler , int clientId);

    int clientId(){return m_clientId;}
    int identifyResponseId(){return m_identifyResponseId;}
    string currRequestFilename(){return m_currRequestFilename;}
protected:
    void onRecvCmd(const unsigned char* buf,const unsigned len) override;
private:
    wxEvtHandler* m_wxEvtHandler;
    int m_clientId;//tcp通讯时使用的ID
    int m_identifyResponseId;//客户端发回的ID，用于Server根据ID来区分发送内容

    bool m_isRecvingFilePartData;

    string m_currRequestFilename;

    void callCmdEventCb(const CmdEventType& event);

};

#endif // SERVERCMDPROCESSER_H
