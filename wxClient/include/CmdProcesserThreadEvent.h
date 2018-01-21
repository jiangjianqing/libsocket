#ifndef CMDPROCESSERTHREADEVENT_H
#define CMDPROCESSERTHREADEVENT_H

#include "wx/event.h"

#define ID_THREADEVENT_CMDPROCESSER wxID_HIGHEST+45//为什么无效*/

class CmdProcesserThreadEvent : public wxThreadEvent
{
public:
    CmdProcesserThreadEvent(wxEventType eventType = wxEVT_THREAD);

    virtual wxEvent *Clone() const wxOVERRIDE
    {
        return new CmdProcesserThreadEvent(*this);
    }
};

#endif // CMDPROCESSERTHREADEVENT_H
