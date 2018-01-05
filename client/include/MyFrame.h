#ifndef MYFRAME_H
#define MYFRAME_H

#include "wx/frame.h"
#include "wx/textctrl.h" //文本框

class MyFrame : public wxFrame
{
    enum
    {
        ID_Hello = 1,
        ID_GetName
    };

public:
    MyFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButtonClick(wxCommandEvent& event);

    wxTextCtrl* txtName;
};

#endif // MYFRAME_H
