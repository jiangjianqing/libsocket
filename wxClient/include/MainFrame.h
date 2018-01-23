#ifndef MainFrame_H
#define MainFrame_H

#include "wx/frame.h"
#include "wx/textctrl.h" //�ı���
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/panel.h"
#include "wx/stattext.h"

#include "tcpclient.h"
#include "UdpBroadcaster.h"
#include "ClientCmdProcesser.h"
#include "CmdBufParser.h"

class MyTray;

//���ַ�ʽ����ID����
#define OKBTNID   wxID_HIGHEST+21

static void Udp_ReadCB(const unsigned char* buf,const unsigned len, void* userdata);
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
        ID_THREADEVENT_TCPCLIENT,//Ϊʲô��Ч����
        ID_THREADEVENT_CMDPROCESSER//Ϊʲô��Ч*/
    };
    void OnClose(wxCloseEvent & event);
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButtonClick(wxCommandEvent& event);
    void OnStopButtonClick(wxCommandEvent& event);

    //�߳��¼�������
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

    ClientCmdProcesser m_cmdProcesser;

    void initSocket();
protected:
    friend void Udp_ReadCB(const unsigned char* buf,const unsigned len, void* userdata);
    friend void Tcp_ReadCB(const unsigned char* buf,const unsigned len, void* userdata);
};

#endif // MainFrame_H
