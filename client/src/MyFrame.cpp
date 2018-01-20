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
#include <functional>
#include <fstream>
#include <sstream>
#include "CmdFactory.h"

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Hello World"),cmdProcesser(this)
{
    m_tray = new MyTray(this);
    m_tray->SetIcon(wxIcon("./icons/lighting-integrate.png"),"Hello Tray!");

    //创建最低层面板,垂直布局
    //wxPanel *topPanel=new wxPanel(this);
    topSizer=new wxBoxSizer(wxVERTICAL);//垂直布局，可用wxHORIZONTAL水平布局替换。
    //topPanel->SetSizer(topSizer);
    this->SetSizer(topSizer);
    //创建上中下三个面板,上下面板高度固定,宽度自由伸展,中间面板上下左右四个方向自由伸展
    upPanel=new wxPanel(this);
    upPanel->SetBackgroundColour(*wxBLACK);//黑色背景
    centerPanel=new wxPanel(this);
    centerPanel->SetBackgroundColour(*wxWHITE);//白色背景
    bottomPanel=new wxPanel(this);
    bottomPanel->SetBackgroundColour(*wxRED);//红色背景

    topSizer->Add(upPanel,0,wxEXPAND);
    topSizer->Add(centerPanel,1,wxEXPAND|wxALL);
    topSizer->Add(bottomPanel,0,wxEXPAND);

/*
    //2018.1.10  windows下尚未找到使用wxMenu应该加载哪个lib文件
    menuFile = new wxMenu;
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
    */
    statusbar = CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);

    //Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);//c++11 lambda
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    pMainToolBar = CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, 6000 );
    //pMainToolBar->AddTool( 6001, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT("") );


    stText = new wxStaticText(upPanel,-1,"test label");
    txtName = new wxTextCtrl(this,-1,"name");//style=wx.TE_MULTILINE  多行样式
    txtPassword = new wxTextCtrl(centerPanel, -1, "password",wxPoint(20,20), wxSize(175, -1),wxTE_PASSWORD);
    txtName->SetInsertionPoint(0);

    btnTest = new wxButton(bottomPanel,ID_GetName,"Test",wxPoint(30,50));
    Bind(wxEVT_BUTTON,&MyFrame::OnButtonClick,this,ID_GetName);

    btnClose = new wxButton(bottomPanel,ID_STOP,"stop");
    Bind(wxEVT_BUTTON,&MyFrame::OnStopButtonClick,this,ID_STOP);

    btnBroadcast = new wxButton(bottomPanel,ID_BROADCAST,"broadcast",wxPoint(80,0));
    Bind(wxEVT_BUTTON,&MyFrame::OnBroadcastButtonClick,this,ID_BROADCAST);

    //Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MyFrame::OnClose));//Connect is deprecated!!!!!
    Bind(wxEVT_CLOSE_WINDOW,&MyFrame::OnClose,this);

    Bind(wxEVT_THREAD,&MyFrame::OnMyThreadEvent,this);
    Bind(wxEVT_THREAD,&MyFrame::onCmdProcesserThreadEvent,this);

    Centre();

    auto tcpCb= [this](AbstractSocket* socket,const socket_event_t& event,const char* buf, int nread,socket_reply_cb reply){
        switch(event.type){
        case socket_event_type::ConnectionAccept:
            onClientAccepted(event.ip,event.port);
            break;
        case socket_event_type::ConnectionClose:
            onClientClosed(event.clientId,event.ip,event.port);
            break;
        case socket_event_type::ReadData:
            break;
        default:
            onSimpleSocketEvent(event.type);
            break;
        }
    };

    auto udpCb= [this](AbstractSocket* socket,const socket_event_t& event,const char* buf, int nread,socket_reply_cb reply){
        switch(event.type){
        case socket_event_type::ConnectionAccept:
            onClientAccepted(event.ip,event.port);
            break;
        case socket_event_type::ConnectionClose:
            onClientClosed(event.clientId,event.ip,event.port);
            break;
        case socket_event_type::ReadData:
            cmdProcesser.recvUdpData(buf,nread,reply);
            break;
        default:
            onSimpleSocketEvent(event.type);
            break;
        }
    };

    server.setSocketEventCb(tcpCb);
    tcpClient.setSocketEventCb(tcpCb);

    udpClient.setSocketEventCb(udpCb);

    //auto f1 = bind(&MyFrame::onClientAccepted,this,placeholders::_1,placeholders::_2);
    //server.setConnectionAcceptedCb(f1);
    //auto f2 = bind(&MyFrame::onClientClosed,this,placeholders::_1,placeholders::_2,placeholders::_3);
    //server.setClientClosedCb(f2);

    cmdProcesser.test();
}

MyFrame::~MyFrame()
{
    if(m_tray){
        delete m_tray;
    }

/* 2018.01.10 手动释放并不能修正wxwidgets的内存泄露
    delete btnTest;
    delete btnClose;

    delete stText;
    delete txtPassword;
    delete bottomPanel;
    delete centerPanel;
    delete upPanel;

    //手动释放以下组件wxFrame会报错
    //delete topSizer;
    //delete menuFile;
    //delete pMainToolBar;
    //delete statusbar;

    */
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

    //client.connect("192.168.18.29",11212);
    //server.start("0.0.0.0",11211);
    udpClient.connect(8899);
    //udpBroadcaster.start(8899);

  /*
 if (choice->GetCurrentSelection() < (int)choice->GetCount() - 1)
  choice->Select(choice->GetCurrentSelection() + 1);
 else
  choice->Select(-1);*/
}

void MyFrame::OnStopButtonClick(wxCommandEvent& event)
{
    INFO("test123456");
    //udpClient.close();
    //udpBroadcaster.close();
    //server.close();
    //client.close();
    wxLogMessage("server stoped!");
}

void MyFrame::OnBroadcastButtonClick(wxCommandEvent& event)
{
    char* buf = nullptr;
    int len = 0;
    if(CmdFactory::makeDiscoverMsg("192.168.18.29",11212,buf,len)){
        string str = CmdFactory::buf2Str(buf,len);
        fstream output("./cmd_log", ios::out | ios::trunc | ios::binary);

        output<<str;
        output.flush();
        output.close();
    }
    INFO("send broadcast!");
    char msg_discovery[] = "123456";
    udpBroadcaster.broadcast(msg_discovery,6);
}

void MyFrame::onClientAccepted(const string& ip,int port)
{
    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    wxThreadEvent event(wxEVT_COMMAND_THREAD, wxID_ANY);
    event.SetId((int)socket_event_type::ConnectionAccept);
    ostringstream ostr;
    ostr<<ip<<":"<<port;
    //string info = std::to_string(port);
    event.SetString(ostr.str());
    wxQueueEvent(this,event.Clone());
    //wxTheApp->QueueEvent(e.Clone());
}

void MyFrame::onClientClosed(int id,const string& ip,int port)
{
    wxThreadEvent event(wxEVT_COMMAND_THREAD, wxID_ANY);
    event.SetId((int)socket_event_type::ConnectionClose);
    event.SetString(std::to_string(id) + ":" + ip + ":" + std::to_string(port));
    wxQueueEvent(this,event.Clone());
}

void MyFrame::onSimpleSocketEvent(socket_event_type type)
{
    wxThreadEvent event(wxEVT_COMMAND_THREAD, wxID_ANY);
    event.SetId((int)type);
    //event.SetString(std::to_string(id) + ":" + ip + ":" + std::to_string(port));
    wxQueueEvent(this,event.Clone());
}

void MyFrame::OnMyThreadEvent(wxThreadEvent& event)
{
    wxString msg = event.GetString();
    switch(event.GetId()){
    case (int)socket_event_type::ConnectionAccept:
        SetStatusText("client accepted : " + msg);
        break;
    case (int)socket_event_type::ConnectFault:
        SetStatusText("client connect fault : ");
        break;
    case (int)socket_event_type::ConnectionClose:
        SetStatusText("client closed : " + msg);
        break;
    case (int)socket_event_type::Listening:
        SetStatusText("server is listening !");
        break;
    case (int)socket_event_type::ListenFault:
        SetStatusText("establish listen fault !");
        break;
    case (int)socket_event_type::Unknow:
        SetStatusText("occur unknow socket event !");
        break;
    case (int)socket_event_type::SocketClose:
        SetStatusText("socket closed !");
        break;
    }

/*
 *
 * //另一个thread中发送消息
    wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    e.SetString(_T("Some string"));
    wxTheApp->QueueEvent(e.Clone());
  */


}

void MyFrame::onCmdProcesserThreadEvent(wxThreadEvent& event)
{
    switch(event.GetId()){
    case (int)CmdEventType::UdpDiscover:
        tcpClient.connect(cmdProcesser.serverIp().c_str(),cmdProcesser.serverPort());
        break;
    }
}
