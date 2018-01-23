#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "tcpserver.h"
#include "UdpBroadcaster.h"
#include "ServerCmdProcesser.h"
#include "CmdBufParser.h"

using namespace std;

static void TcpServer_NewConnect(int clientid, void* userdata);

class MainFrame : public wxFrame
{
public:
    enum
    {
        ID_BTN_START = wxID_HIGHEST + 1,
        ID_BTN_SELECTFILE,
        ID_BTN_BROADCAST,
        ID_TEXT_FILENAME,
        ID_TEXT_INFO
    };
    explicit MainFrame();

protected:
//--------wxwidget event---
    void OnExit(wxCommandEvent& event);


//--------------自定义event-----------
    void OnClose(wxCloseEvent & event);
    void onBtnSelectFileClick(wxCommandEvent& event);
    void onBtnBroadcastClick(wxCommandEvent& event);
    void onBtnStartClick(wxCommandEvent& event);

    //线程事件处理函数
    void onSocketThreadEvent(wxThreadEvent& event);

    friend void TcpServer_NewConnect(int clientid, void* userdata);
private:
    void initSocket();
    wxButton* m_btnStart;
    wxButton* m_btnSelectFile;
    wxButton* m_btnBroadcast;
    wxTextCtrl* m_txtInfo;
    wxTextCtrl* m_txtFilename;

    wxPanel *m_bottomPanel;
    wxPanel *m_centerPanel;
    wxPanel *m_topPanel;
    wxBoxSizer *m_mainSizer;
    wxBoxSizer* m_bottomSizer;
    wxBoxSizer* m_centerSizer;

    uv::TcpServer m_tcpServer;
    uv::UdpClient m_udpBroadcaster;

    ServerCmdProcesser m_cmdProcesser;

    CmdBufParser m_cmdBufParser;

    bool m_isRunning;
    void appendInfo(const string& info);
};

#endif // MAINFRAME_H
