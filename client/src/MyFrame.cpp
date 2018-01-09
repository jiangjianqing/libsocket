#include "MyFrame.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/log.h"
#include "wx/stattext.h"

#include "wx/button.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/toolbar.h"
#include "MyTray.h"


MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Hello World")
{
    m_tray = new MyTray(this);
    m_tray->SetIcon(wxIcon("./icons/lighting-integrate.png"),"Hello Tray!");

    //创建最低层面板,垂直布局
    //wxPanel *topPanel=new wxPanel(this);
    wxBoxSizer *topSizer=new wxBoxSizer(wxVERTICAL);//垂直布局，可用wxHORIZONTAL水平布局替换。
    //topPanel->SetSizer(topSizer);
    this->SetSizer(topSizer);
    //创建上中下三个面板,上下面板高度固定,宽度自由伸展,中间面板上下左右四个方向自由伸展
    wxPanel *upPanel=new wxPanel(this);
    upPanel->SetBackgroundColour(*wxBLACK);//黑色背景
    wxPanel *centerPanel=new wxPanel(this);
    centerPanel->SetBackgroundColour(*wxWHITE);//白色背景
    wxPanel *bottomPanel=new wxPanel(this);
    bottomPanel->SetBackgroundColour(*wxRED);//红色背景

    topSizer->Add(upPanel,0,wxEXPAND);
    topSizer->Add(centerPanel,1,wxEXPAND|wxALL);
    topSizer->Add(bottomPanel,0,wxEXPAND);


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

    wxToolBar* pMainToolBar = CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, 6000 );
    //pMainToolBar->AddTool( 6001, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT("") );


    wxStaticText* stText = new wxStaticText(upPanel,-1,"test label");
    txtName = new wxTextCtrl(this,-1,"name");//style=wx.TE_MULTILINE  多行样式
    wxTextCtrl* txtPassword = new wxTextCtrl(centerPanel, -1, "password",wxPoint(20,20), wxSize(175, -1),wxTE_PASSWORD);
    txtName->SetInsertionPoint(0);

    wxButton* btnTest = new wxButton(bottomPanel,ID_GetName,"Test",wxPoint(30,50));
    Bind(wxEVT_BUTTON,&MyFrame::OnButtonClick,this,ID_GetName);

    wxButton* btnClose = new wxButton(bottomPanel,ID_STOP,"stop");
    Bind(wxEVT_BUTTON,&MyFrame::OnStopButtonClick,this,ID_STOP);

    //Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MyFrame::OnClose));//Connect is deprecated!!!!!
    Bind(wxEVT_CLOSE_WINDOW,&MyFrame::OnClose,this);

    Centre();

}

MyFrame::~MyFrame()
{
    if(m_tray){
        delete m_tray;
    }
}

void MyFrame::OnClose(wxCloseEvent & event)
{
    /*
    if(wxMessageBox(wxT("用wxT支持中文，Are you sure to Quit?"),"Question",wxYES_NO | wxICON_QUESTION) != wxYES){
        //event.Skip();//event.Skip()方法是将此事件向MainFrame的父级传递，使用上要注意
        //此时MainApp中的事 件处理也会被触发，如果我们不使用event.Skip()，则事件在MainFrame中被处理后就停止发送了。当然这里调用Skip，只是为了加深理 解wxWidgets的事件处理机制，在实际应用中，视具体需求而定
        event.Veto();
        return;
    }
    */

    wxMessageDialog * dial = new wxMessageDialog(NULL, _T("Are you sure to quit?"),
                                                     _T("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
    int ret = dial->ShowModal();
    dial->Destroy();
    //注意:要销毁一个窗口，必须要调用它的Destroy()方法。
    if(ret == wxID_YES){
        Destroy();
    }else{
        event.Veto();
    }
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    //The parameter true indicates that other windows have no veto power such as after asking "Do you really want to close?".
    //If there is no other main window left, the application will quit
    Close();
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

    server.start("0.0.0.0",11211);
  /*
 if (choice->GetCurrentSelection() < (int)choice->GetCount() - 1)
  choice->Select(choice->GetCurrentSelection() + 1);
 else
  choice->Select(-1);*/
}

void MyFrame::OnStopButtonClick(wxCommandEvent& event)
{
    server.close();
    wxLogMessage("server stoped!");
}
