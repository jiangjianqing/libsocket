#ifndef TCPCLIENTTHREADEVENT_H
#define TCPCLIENTTHREADEVENT_H

#include "wx/event.h"

#define ID_THREADEVENT_TCPCLIENT wxID_HIGHEST+46//为什么无效*/

class TcpClientThreadEvent : public wxThreadEvent
{
public:
    TcpClientThreadEvent(wxEventType eventType = wxEVT_THREAD);

    virtual wxEvent *Clone() const wxOVERRIDE
    {
        return new TcpClientThreadEvent(*this);
    }
};

#endif // TCPCLIENTTHREADEVENT_H
