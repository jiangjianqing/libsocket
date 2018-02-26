#include "CmdProcessorThreadEvent.h"

CmdProcessorThreadEvent::CmdProcessorThreadEvent(ServerCmdProcessor* cmdProcessor)
    : wxThreadEvent(wxEVT_COMMAND_THREAD,wxID_ANY),m_cmdProcessor(cmdProcessor)
{

}

CmdProcessorThreadEvent::CmdProcessorThreadEvent(const CmdProcessorThreadEvent& lvalue)
    : wxThreadEvent(lvalue)
{
    m_cmdProcessor = lvalue.cmdProcessor();
}
