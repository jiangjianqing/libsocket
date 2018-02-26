#ifndef SERVERCMDPROCESSOR_H
#define SERVERCMDPROCESSOR_H

#include "wx/event.h"
#include "AbstractServerCmdProcessor.h"

class ServerCmdProcessor : public AbstractServerCmdProcessor
{
public:
    explicit ServerCmdProcessor(wxEvtHandler* evtHandler , int clientId);

protected:
    void callCmdEventCb(const int& eventTypeId) override;
private:
    wxEvtHandler* m_wxEvtHandler;
};

#endif // SERVERCMDPROCESSOR_H
