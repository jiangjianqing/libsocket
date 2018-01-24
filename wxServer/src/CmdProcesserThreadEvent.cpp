#include "CmdProcesserThreadEvent.h"

CmdProcesserThreadEvent::CmdProcesserThreadEvent(ServerCmdProcesser* cmdProcesser)
    : wxThreadEvent(wxEVT_COMMAND_THREAD,wxID_ANY),m_cmdProcesser(cmdProcesser)
{

}

CmdProcesserThreadEvent::CmdProcesserThreadEvent(const CmdProcesserThreadEvent& lvalue)
    : wxThreadEvent(lvalue)
{
    m_cmdProcesser = lvalue.cmdProcesser();
}
