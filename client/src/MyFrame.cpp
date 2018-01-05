#include "MyFrame.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/log.h"
#include "wx/stattext.h"

#include "wx/button.h"

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Hello World")
{

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);

    //Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);//c++11 lambda
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);


    wxStaticText* stText = new wxStaticText(this,-1,"test label");
    txtName = new wxTextCtrl(this,-1,"name");//style=wx.TE_MULTILINE  多行样式
    wxTextCtrl* txtPassword = new wxTextCtrl(this, -1, "password",wxPoint(20,20), wxSize(175, -1),wxTE_PASSWORD);
    txtName->SetInsertionPoint(0);

    wxButton* btnTest = new wxButton(this,ID_GetName,"Test",wxPoint(30,50));
    Bind(wxEVT_BUTTON,&MyFrame::OnButtonClick,this,ID_GetName);

}

void MyFrame::OnExit(wxCommandEvent& event)
{
    //The parameter true indicates that other windows have no veto power such as after asking "Do you really want to close?".
    //If there is no other main window left, the application will quit
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::OnButtonClick(wxCommandEvent& event)
{

    wxString str =  txtName->GetLineText(0);
    wxLogMessage("The name is : " + str);
    txtName->SetValue("11223344");//改变文本框的内容
  /*
 if (choice->GetCurrentSelection() < (int)choice->GetCount() - 1)
  choice->Select(choice->GetCurrentSelection() + 1);
 else
  choice->Select(-1);*/
}
