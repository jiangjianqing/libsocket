#ifndef TCPCLIENTTHREADEVENT_H
#define TCPCLIENTTHREADEVENT_H

#include "wx/event.h"

class ServerCmdProcessor;

class CmdProcessorThreadEvent : public wxThreadEvent
{
public:
    CmdProcessorThreadEvent(ServerCmdProcessor* cmdProcessor);
    CmdProcessorThreadEvent(const CmdProcessorThreadEvent& event);

    ServerCmdProcessor* cmdProcessor() const {return m_cmdProcessor;}

    virtual wxEvent *Clone() const wxOVERRIDE
    {
        return new CmdProcessorThreadEvent(*this);
    }

private:
    ServerCmdProcessor* m_cmdProcessor;
};

#endif // TCPCLIENTTHREADEVENT_H
