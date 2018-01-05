#ifndef MYFRAME_H
#define MYFRAME_H

#include "wx/frame.h"

class MyFrame : public wxFrame
{
    enum
    {
        ID_Hello = 1
    };

public:
    MyFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

#endif // MYFRAME_H
