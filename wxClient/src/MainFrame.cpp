#include "MainFrame.h"
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
#include "ProtobufUtils.h"
#include "TcpClientThreadEvent.h"
#include "CmdProcesserThreadEvent.h"

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "Updater"),cmdProcesser(this)
{
    //Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);//c++11 lambda
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    //Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));//Connect is deprecated!!!!!
    Bind(wxEVT_CLOSE_WINDOW,&MainFrame::OnClose,this);

    m_tray = new MyTray(this);
    m_tray->SetIcon(wxIcon("./icons/lighting-integrate.png"),"Hello Tray!");

    //创建最低层面板,垂直布局
    //wxPanel *topPanel=new wxPanel(this);
    wxBoxSizer* m_mainSizer=new wxBoxSizer(wxVERTICAL);//垂直布局，可用wxHORIZONTAL水平布局替换。
    //topPanel->SetSizer(topSizer);
    this->SetSizer(m_mainSizer);
    //创建上中下三个面板,上下面板高度固定,宽度自由伸展,中间面板上下左右四个方向自由伸展
    upPanel=new wxPanel(this);
    upPanel->SetBackgroundColour(*wxBLACK);//黑色背景
    m_centerPanel=new wxPanel(this);
    m_centerPanel->SetBackgroundColour(*wxWHITE);//白色背景
    bottomPanel=new wxPanel(this);
    bottomPanel->SetBackgroundColour(*wxRED);//红色背景

    m_mainSizer->Add(upPanel,0,wxEXPAND);
    m_mainSizer->Add(m_centerPanel,1,wxEXPAND);
    m_mainSizer->Add(bottomPanel,0,wxEXPAND);

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
    Bind(wxEVT_MENU, &MainFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);

    pMainToolBar = CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, 6000 );
    //pMainToolBar->AddTool( 6001, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT("") );

    stText = new wxStaticText(upPanel,-1,"test label");
    m_txtInfo = new wxTextCtrl(m_centerPanel,-1,"name",wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY);//style=wx.TE_MULTILINE  多行样式
    //txtPassword = new wxTextCtrl(centerPanel, -1, "password",wxPoint(20,20), wxSize(175, -1),wxTE_PASSWORD);
    m_txtInfo->SetInsertionPoint(0);

    wxBoxSizer* centerSizer = new wxBoxSizer(wxVERTICAL);
    m_centerPanel->SetSizer(centerSizer);
    centerSizer->Add(m_txtInfo,1,wxEXPAND|wxALL,10);

    btnTest = new wxButton(bottomPanel,ID_GetName,"Test",wxPoint(30,50));
    Bind(wxEVT_BUTTON,&MainFrame::OnButtonClick,this,ID_GetName);

    btnClose = new wxButton(bottomPanel,ID_STOP,"stop");
    Bind(wxEVT_BUTTON,&MainFrame::OnStopButtonClick,this,ID_STOP);

    Centre();

    initSocket();
    cmdProcesser.test();
}

MainFrame::~MainFrame()
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

void MainFrame::initSocket()
{
    //2018.01.21 目前将两个ThreadEvent放在一个事件中处理，通过dynamic_cast来区分
    //原因是通过Bind + ID来区分的方式未生效
    INFO("为什么 Bind + ID 区分的方式在Thread上没有生效，而普通Button却可以？？？");
    Bind(wxEVT_THREAD,&MainFrame::onThreadEvent,this);

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
            onSimpleTcpSocketEvent(event.type);
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
            onSimpleTcpSocketEvent(event.type);
            break;
        }
    };

    tcpClient.setSocketEventCb(tcpCb);

    udpClient.setSocketEventCb(udpCb);

    //auto f1 = bind(&MainFrame::onClientAccepted,this,placeholders::_1,placeholders::_2);
    //server.setConnectionAcceptedCb(f1);
    //auto f2 = bind(&MainFrame::onClientClosed,this,placeholders::_1,placeholders::_2,placeholders::_3);
    //server.setClientClosedCb(f2);
    udpClient.connect(8899);
}

void MainFrame::OnClose(wxCloseEvent & event)
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

void MainFrame::OnExit(wxCommandEvent& event)
{
    //The parameter true indicates that other windows have no veto power such as after asking "Do you really want to close?".
    //If there is no other main window left, the application will quit
    Close();
}
void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void MainFrame::OnHello(wxCommandEvent& event)
{

    wxLogMessage("Hello world from wxWidgets!");
}

void MainFrame::OnButtonClick(wxCommandEvent& event)
{

    wxString str =  m_txtInfo->GetLineText(0);
    wxLogMessage("The name is : " + str);
    m_txtInfo->SetValue("11223344");//改变文本框的内容

    tcpClient.connect("127.0.0.1",11212);
    //server.start("0.0.0.0",11211);
    //udpClient.connect(8899);
    //udpBroadcaster.start(8899);

  /*
 if (choice->GetCurrentSelection() < (int)choice->GetCount() - 1)
  choice->Select(choice->GetCurrentSelection() + 1);
 else
  choice->Select(-1);*/
}

void MainFrame::OnStopButtonClick(wxCommandEvent& event)
{
    INFO("test123456");
    //udpClient.close();
    //udpBroadcaster.close();
    //server.close();
    tcpClient.close();
    wxLogMessage("server stoped!");
}

void MainFrame::onClientAccepted(const string& ip,int port)
{
    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    //wxThreadEvent event(wxEVT_COMMAND_THREAD, ID_THREADEVENT_TCPCLIENT);
    TcpClientThreadEvent event;
    event.SetId((int)socket_event_type::ConnectionAccept);
    ostringstream ostr;
    ostr<<ip<<":"<<port;
    //string info = std::to_string(port);
    event.SetString(ostr.str());
    wxQueueEvent(this,event.Clone());
    //wxTheApp->QueueEvent(e.Clone());
}

void MainFrame::onClientClosed(int id,const string& ip,int port)
{
    //wxThreadEvent event(wxEVT_COMMAND_THREAD, ID_THREADEVENT_TCPCLIENT);
    TcpClientThreadEvent event;
    event.SetId((int)socket_event_type::ConnectionClose);
    event.SetString(std::to_string(id) + ":" + ip + ":" + std::to_string(port));
    wxQueueEvent(this,event.Clone());
}

void MainFrame::onSimpleTcpSocketEvent(socket_event_type type)
{
    //wxThreadEvent event(wxEVT_COMMAND_THREAD, ID_THREADEVENT_TCPCLIENT);
    TcpClientThreadEvent event;
    event.SetId((int)type);
    //event.SetString(std::to_string(id) + ":" + ip + ":" + std::to_string(port));
    wxQueueEvent(this,event.Clone());
}

void MainFrame::onThreadEvent(wxThreadEvent& event)
{
    if(dynamic_cast<TcpClientThreadEvent*>(&event) != nullptr){
        wxString msg = event.GetString();
        int i = event.GetId();
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
        return;
    }

    if(dynamic_cast<CmdProcesserThreadEvent*>(&event) != nullptr){
        switch(event.GetId()){
        case (int)CmdEventType::UdpDiscover:
            tcpClient.connect(cmdProcesser.serverIp().c_str(),cmdProcesser.serverPort());
            break;
        }

        return;
    }
/*
 *
 * //另一个thread中发送消息
    wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    e.SetString(_T("Some string"));
    wxTheApp->QueueEvent(e.Clone());
  */
}
