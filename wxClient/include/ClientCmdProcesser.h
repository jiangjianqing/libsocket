#ifndef CMDPROCESSER_H
#define CMDPROCESSER_H

#include <string>
#include <vector>
#include <functional>
#include "wx/event.h"
#include "LuaEngine.h"
#include "CmdBufParser.h"

using namespace std;

enum class CmdEventType{
    UNKNOW,UdpDiscover,TcpIdentifyResponse,TcpFileListRequest,TcpSendFileRequest
};

#define ID_CMDPROCESSER_THREADEVENT   wxID_HIGHEST+30

typedef function<void (const CmdEventType& event)> CmdEventCb;

class ClientCmdProcesser : public CmdBufParser
{
public:
    explicit ClientCmdProcesser(wxEvtHandler* evtHandler);

    void callCmdEventCb(const CmdEventType& event);

    void recvUdpData(const char* buf,int nread);

    string serverIp(){return m_serverIp;}
    int serverPort(){return m_serverPort;}
    //const vector<string>& filenameList(){return m_filenameList;}
    string getCurrFilename();
    bool toNextFilename();

    void protobuf_test(const char* msg_type_name,const char* buf,size_t len);

protected:
    void onRecvCmd(const unsigned char* buf,const unsigned len) override;
private:
    wxEvtHandler* m_wxEvtHandler;
    string m_serverIp;
    int m_serverPort;
    LuaEngine m_luaEngine;
    //特别注意，该路径一定要是某个子目录，因为后续会对其进行删除清空
    string m_recvFilePath;

    vector<string> m_filenameList;
    int m_CurrFileNameListIndex;//当前待获取文件名的index
};

#endif // CMDPROCESSER_H
