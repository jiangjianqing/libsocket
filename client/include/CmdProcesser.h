#ifndef CMDPROCESSER_H
#define CMDPROCESSER_H

#include <string>
#include <functional>
#include "SocketData.h"
#include "wx/event.h"

using namespace std;

enum class CmdEventType{
    UNKNOW,UdpDiscover
};

#define ID_CMDPROCESSER_THREADEVENT   wxID_HIGHEST+30

typedef function<void (const CmdEventType& event)> CmdEventCb;

class CmdProcesser
{
public:
    explicit CmdProcesser(wxEvtHandler* evtHandler);

    void callCmdEventCb(const CmdEventType& event);

    void recvUdpData(const char* buf,int nread,socket_reply_cb replyCb);

    string serverIp(){return m_serverIp;}
    int serverPort(){return m_serverPort;}


private:
    wxEvtHandler* m_wxEvtHandler;
    string m_serverIp;
    int m_serverPort;
};

#endif // CMDPROCESSER_H
