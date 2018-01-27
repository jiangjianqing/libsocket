#ifndef MainFrame_H
#define MainFrame_H

#include "wx/frame.h"
#include "wx/textctrl.h" //文本框
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/panel.h"
#include "wx/stattext.h"

#define BUFFER_SIZE (1024*512) //重要：在#include "tcpclient.h"之前设置缓冲区参数
#include "tcpclient.h"
#include "UdpBroadcaster.h"
#include "ClientCmdProcessor.h"
#include "CmdBufParser.h"

class MyTray;

//这种方式定义ID更好
#define OKBTNID   wxID_HIGHEST+21

static void Udp_ReadCB(int remoteId,const unsigned char* buf,const unsigned len, void* userdata);
static void Tcp_ReadCB(const unsigned char* buf,const unsigned len, void* userdata);

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
        ID_STOP/*,
        ID_THREADEVENT_TCPCLIENT,//为什么无效？？
        ID_THREADEVENT_CMDPROCESSER//为什么无效*/
    };
    void OnClose(wxCloseEvent & event);
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButtonClick(wxCommandEvent& event);
    void OnStopButtonClick(wxCommandEvent& event);

    //线程事件处理函数
    void onThreadEvent(wxThreadEvent& event);

    void onClientAccepted(const string& ip,int port);
    void onClientClosed(int id,const string& ip,int port);
    void onRecvCmd(const unsigned char* buf,const unsigned len);

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

    uv::TcpClient m_tcpClient;
    uv::UdpClient m_udpClient;

    ClientCmdProcessor m_cmdProcessor;

    void initSocket();
    void appendInfo(const wxString& info);
protected:
    friend void Udp_ReadCB(int remoteId,const unsigned char* buf,const unsigned len, void* userdata);
    friend void Tcp_ReadCB(const unsigned char* buf,const unsigned len, void* userdata);
private:
    string m_remoteServerIp;
};

#endif // MainFrame_H
