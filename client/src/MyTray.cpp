#include "MyTray.h"


MyTray::MyTray(MyFrame* mainFrame) : wxTaskBarIcon(),m_mainFrame(mainFrame)
{

}

wxMenu *MyTray::CreatePopupMenu()
{
    // 每次都要 new 一个新的 wxMenu
    wxMenu *popupMenu = new wxMenu;
    popupMenu->Append(wxID_ABOUT, _("About"));
    popupMenu->AppendSeparator();
    popupMenu->Append(wxID_EXIT, _("Exit"));

    Bind(wxEVT_MENU, &MyTray::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyTray::OnExit, this, wxID_EXIT);

    return popupMenu;
}


void MyTray::OnAbout(wxCommandEvent& event)
{
    m_mainFrame->OnAbout(event);
    //wxMessageBox(_T("hello!"), wxT("About Tray!"), wxOK | wxICON_INFORMATION);
}

void MyTray::OnExit(wxCommandEvent& event)
{
    m_mainFrame->OnExit(event);
    //wxMessageBox(_T("hello!"), wxT("About Tray!"), wxOK | wxICON_INFORMATION);
}


/*
template <typename EventTag, typename Class, typename EventArg>
void MyTray::bindClose(const EventTag &eventType,void (Class::*method)(EventArg &))
{
    method();
}
*/

