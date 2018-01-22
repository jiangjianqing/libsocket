#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "TcpServer.h"
#include "UdpBroadcaster.h"
#include "ServerCmdProcesser.h"

using namespace std;

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

    void onClientAccepted(const string& ip,int port);
    void onClientClosed(int id,const string& ip,int port);
    void onSimpleTcpSocketEvent(socket_event_type type);
    //线程事件处理函数
    void onSocketThreadEvent(wxThreadEvent& event);
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

    TcpServer m_tcpServer;
    UdpBroadcaster m_udpBroadcaster;

    ServerCmdProcesser m_cmdProcesser;

    bool m_isRunning;
    void appendInfo(const string& info);
};

#endif // MAINFRAME_H
