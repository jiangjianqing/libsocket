#ifndef MainFrame_H
#define MainFrame_H

#include "wx/frame.h"
#include "wx/textctrl.h" //文本框
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/panel.h"
#include "wx/stattext.h"

#include "TcpClient.h"
#include "UdpClient.h"
#include "CmdProcesser.h"

class MyTray;

//这种方式定义ID更好
#define OKBTNID   wxID_HIGHEST+21

class MainFrame : public wxFrame
{
    friend class MyTray;
public:


    explicit MainFrame();
    virtual ~MainFrame();

private:
    enum
    {
        ID_Hello = wxID_HIGHEST + 1,
        ID_GetName,
        ID_STOP,
        ID_THREAD_TCPCLIENT
    };
    void OnClose(wxCloseEvent & event);
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButtonClick(wxCommandEvent& event);
    void OnStopButtonClick(wxCommandEvent& event);

    //线程事件处理函数
    void onSocketThreadEvent(wxThreadEvent& event);
    void onCmdProcesserThreadEvent(wxThreadEvent& event);

    void onClientAccepted(const string& ip,int port);
    void onClientClosed(int id,const string& ip,int port);
    void onSimpleTcpSocketEvent(socket_event_type type);

    wxTextCtrl* m_txtInfo;
    MyTray* m_tray;

    wxButton* btnTest;
    wxButton* btnClose;

    wxStaticText* stText;
    wxTextCtrl* txtPassword;
    wxToolBar* pMainToolBar;
    //wxMenu *menuFile;

    wxPanel *bottomPanel;
    wxPanel * m_centerPanel;
    wxPanel *upPanel;
    wxStatusBar* statusbar;

    TcpClient tcpClient;
    UdpClient udpClient;

    CmdProcesser cmdProcesser;

    void initSocket();
};

#endif // MainFrame_H
