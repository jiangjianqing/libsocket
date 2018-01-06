#ifndef MYFRAME_H
#define MYFRAME_H

#include "wx/frame.h"
#include "wx/textctrl.h" //文本框
#include "MyTray.h"

//这种方式定义ID更好
#define OKBTNID   wxID_HIGHEST+21

class MyFrame : public wxFrame
{
    enum
    {
        ID_Hello = wxID_HIGHEST + 1,
        ID_GetName
    };

public:
    explicit MyFrame();
    virtual ~MyFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButtonClick(wxCommandEvent& event);

    wxTextCtrl* txtName;
    MyTray* m_tray;
};

#endif // MYFRAME_H
