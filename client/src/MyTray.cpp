#include "MyTray.h"

MyTray::MyTray() : wxTaskBarIcon()
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

    return popupMenu;
}

void MyTray::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(_T("hello!"), wxT("About Tray!"), wxOK | wxICON_INFORMATION);
}
