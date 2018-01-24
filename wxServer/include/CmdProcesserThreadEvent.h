#ifndef TCPCLIENTTHREADEVENT_H
#define TCPCLIENTTHREADEVENT_H

#include "wx/event.h"

class ServerCmdProcesser;

class CmdProcesserThreadEvent : public wxThreadEvent
{
public:
    CmdProcesserThreadEvent(ServerCmdProcesser* cmdProcesser);
    CmdProcesserThreadEvent(const CmdProcesserThreadEvent& event);

    ServerCmdProcesser* cmdProcesser() const {return m_cmdProcesser;}

    virtual wxEvent *Clone() const wxOVERRIDE
    {
        return new CmdProcesserThreadEvent(*this);
    }

private:
    ServerCmdProcesser* m_cmdProcesser;
};

#endif // TCPCLIENTTHREADEVENT_H
