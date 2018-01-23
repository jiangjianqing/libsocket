#ifndef CMDPROCESSER_H
#define CMDPROCESSER_H

#include <string>
#include <functional>
#include "wx/event.h"
#include "LuaEngine.h"
#include "CmdBufParser.h"

using namespace std;

enum class CmdEventType{
    UNKNOW,UdpDiscover
};

#define ID_CMDPROCESSER_THREADEVENT   wxID_HIGHEST+30

typedef function<void (const CmdEventType& event)> CmdEventCb;

class ClientCmdProcesser
{
public:
    explicit ClientCmdProcesser(wxEvtHandler* evtHandler);

    void callCmdEventCb(const CmdEventType& event);

    void recvUdpData(const char* buf,int nread);
    void recvTcpData(const char* buf,int nread);

    string serverIp(){return m_serverIp;}
    int serverPort(){return m_serverPort;}

    void test();
    void protobuf_test(const char* msg_type_name,const char* buf,size_t len);


private:
    wxEvtHandler* m_wxEvtHandler;
    string m_serverIp;
    int m_serverPort;
    CmdBufParser m_cmdParser;
    LuaEngine m_luaEngine;

    void onRecvTcpCmd(const unsigned char* buf,const unsigned len);
};

#endif // CMDPROCESSER_H
