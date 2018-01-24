#ifndef SERVERCMDPROCESSER_H
#define SERVERCMDPROCESSER_H

#include "wx/event.h"
#include "CmdBufParser.h"

class ServerCmdProcesser : public CmdBufParser
{
public:
    explicit ServerCmdProcesser(wxEvtHandler* evtHandler , int clientId);
protected:
    void onRecvCmd(const unsigned char* buf,const unsigned len) override;
private:
    wxEvtHandler* m_wxEvtHandler;
    int m_clientId;

    bool m_isRecvingFilePartData;

};

#endif // SERVERCMDPROCESSER_H
