#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "TcpServer.h"
#include "UdpBroadcaster.h"

class MainFrame : public wxFrame
{
public:
    enum
    {
        ID_BTN_START = wxID_HIGHEST + 1,
        ID_BTN_SELECTFILE,
        ID_BTN_TEST,
        ID_TEXT_FILENAME,
        ID_TEXT_INFO
    };
    explicit MainFrame();

protected:
    void OnBtnSelectFileClick(wxCommandEvent& event);
private:
    wxButton* m_btnStart;
    wxButton* m_btnSelectFile;
    wxButton* m_btnTest;
    wxTextCtrl* m_txtInfo;
    wxTextCtrl* m_txtFilename;

    wxPanel *m_bottomPanel;
    wxPanel *m_centerPanel;
    wxPanel *m_topPanel;
    wxBoxSizer *m_mainSizer;
    wxBoxSizer* m_bottomSizer;
    wxBoxSizer* m_centerSizer;

    TcpServer m_server;
    UdpBroadcaster m_udpBroadcaster;
};

#endif // MAINFRAME_H
