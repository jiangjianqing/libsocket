#ifndef MYFRAME_H
#define MYFRAME_H

#include "wx/frame.h"
#include "wx/textctrl.h" //文本框

#include "TcpServer.h"

class MyTray;

//这种方式定义ID更好
#define OKBTNID   wxID_HIGHEST+21

class MyFrame : public wxFrame
{
    friend class MyTray;
public:


    explicit MyFrame();
    virtual ~MyFrame();

private:
    enum
    {
        ID_Hello = wxID_HIGHEST + 1,
        ID_GetName,
        ID_STOP
    };
    void OnClose(wxCloseEvent & event);
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButtonClick(wxCommandEvent& event);
    void OnStopButtonClick(wxCommandEvent& event);

    wxTextCtrl* txtName;
    MyTray* m_tray;

    TcpServer server;
};

#endif // MYFRAME_H
