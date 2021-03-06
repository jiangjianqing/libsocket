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
#include <thread>

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "Updater"),m_cmdProcessor(this)
{
    //Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);//c++11 lambda
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    //Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));//Connect is deprecated!!!!!
    Bind(wxEVT_CLOSE_WINDOW,&MainFrame::OnClose,this);

    m_tray = new MyTray(this);

#if defined (WIN32) || defined(_WIN32)
#include <windows.h>
    //windows下使用资源ID
    m_tray->SetIcon(wxIcon("IDI_ICON1"),"Hello Updater!");
    //m_tray->SetIcon(wxIcon("d:\\Delete.png"),"Hello Tray!");
#elif defined __linux__
    //linux下可以使用路径
    m_tray->SetIcon(wxIcon("./icons/lighting-integrate.png"),"Hello Updater!");
#endif

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
    //SetStatusText("Welcome to wxWidgets!");
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
/*
    btnTest = new wxButton(bottomPanel,ID_GetName,"Test",wxPoint(30,50));
    Bind(wxEVT_BUTTON,&MainFrame::OnButtonClick,this,ID_GetName);

    btnClose = new wxButton(bottomPanel,ID_STOP,"stop");
    Bind(wxEVT_BUTTON,&MainFrame::OnStopButtonClick,this,ID_STOP);
    */

    Centre();

    initSocket();
}

MainFrame::~MainFrame()
{
    m_udpClient.close();
    m_tcpClient.close();
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

void Tcp_ReadCB(const unsigned char* buf,const unsigned len, void* userdata)
{
    MainFrame* mainFrame = reinterpret_cast<MainFrame*>(userdata);
    mainFrame->m_cmdProcessor.recvData((const char*)buf,len);
}

void Udp_ReadCB(int remoteId,const unsigned char* buf,const unsigned len, void* userdata)
{
    MainFrame* mainFrame = reinterpret_cast<MainFrame*>(userdata);
    remote_info_t* info = mainFrame->m_udpClient.getRemoteInfoById(remoteId);
    mainFrame->m_remoteServerIp = info->ip4;
    mainFrame->m_cmdProcessor.recvUdpData((const char*)buf,len);
}

void MainFrame::initSocket()
{
    //2018.01.21 目前将两个ThreadEvent放在一个事件中处理，通过dynamic_cast来区分
    //原因是通过Bind + ID来区分的方式未生效
    fprintf(stdout,"为什么 Bind + ID 区分的方式在Thread上没有生效，而普通Button却可以？？？");
    Bind(wxEVT_THREAD,&MainFrame::onThreadEvent,this);

    m_tcpClient.setNoDelay(true);
    m_tcpClient.setRecvCB(Tcp_ReadCB,this);

    m_udpClient.setRecvCB(Udp_ReadCB,this);
    m_udpClient.connect("0.0.0.0",8899);//ip设为0.0.0.0就可以作为UdpServer使用
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
    //测试dump
    void * p = nullptr;
    ((wxButton*)p)->GetBestVirtualSize();
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
    m_tcpClient.send("1234567890",10);

    //tcpClient.connect("127.0.0.1",11212);
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
    m_udpClient.close();
    m_tcpClient.close();

    wxLogMessage("server stoped!");
}

void MainFrame::onClientAccepted(const string& ip,int port)
{
    //wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    //wxThreadEvent event(wxEVT_COMMAND_THREAD, ID_THREADEVENT_TCPCLIENT);
    TcpClientThreadEvent event;
    //event.SetId((int)socket_event_type::ConnectionAccept);
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
    //event.SetId((int)socket_event_type::ConnectionClose);
    event.SetString(std::to_string(id) + ":" + ip + ":" + std::to_string(port));
    wxQueueEvent(this,event.Clone());
}

void MainFrame::onThreadEvent(wxThreadEvent& event)
{
    if(dynamic_cast<TcpClientThreadEvent*>(&event) != nullptr){
        wxString msg = event.GetString();
        int i = event.GetId();
        return;
    }

    if(dynamic_cast<CmdProcesserThreadEvent*>(&event) != nullptr){
        switch(event.GetId()){
        case (int)ClientCmdEventType::UdpDiscover:{
            string remoteIp = m_remoteServerIp;
            thread t1{[this,remoteIp](){
                 m_tcpClient.connect(remoteIp.c_str(),m_cmdProcessor.serverPort());
            }};
            t1.detach();
            }
            break;
        case (int)ClientCmdEventType::TcpIdentifyResponse:{
            int id = m_cmdProcessor.identifyResponseId();
            appendInfo("identifyResponseId = " + std::to_string(id));
            thread t1{[this,id](){
                    unsigned char* buf = nullptr;
                    unsigned len = 0;
                    CmdFactory::makeIdentifyResponseMsg(id,buf,len);
                    m_tcpClient.send((const char*)buf,len);
                    free(buf);
            }};//end of thread;
            t1.detach();
            }
            break;
        case (int)ClientCmdEventType::TcpFileListRequest:{
            appendInfo(wxT("ready to download file list."));
            thread t1{[this](){
                    unsigned char* buf = nullptr;
                    unsigned len = 0;
                    CmdFactory::makeFileListRequest(m_cmdProcessor.identifyResponseId(),buf,len);
                    m_tcpClient.send((const char*)buf,len);
                    free(buf);
            }};//end of thread
            t1.detach();
            }
            break;
        case (int)ClientCmdEventType::TcpSendFileRequest_NextFile:{
            wstring filename = L"";
            uint64_t startpos = -1;
            if(m_cmdProcessor.toNextFilename()){
                m_cmdProcessor.getCurrRequestFileInfo(filename,startpos);
            }
            if(!filename.empty()){
                appendInfo(wxT("starting download file :") + filename);
            }else{
                appendInfo(wxT("all file download."));
            }
            thread t1{[this,filename](){
                    unsigned char* buf = nullptr;
                    unsigned len = 0;
                    if(filename.empty() == false){
                        CmdFactory::makeSendFileRequest(filename,0,buf,len);//从0位置获取文件数据
                    }else{
                        ///todo:如果没有文件需要发送,则通知服务器FileList获取成功
                        CmdFactory::makeFileListSendResultResponse(true,vector<string>(),buf,len);
                    }
                    m_tcpClient.send((const char*)buf,len);
                    free(buf);
            }};
            t1.detach();

            }//end of case
            break;
        case (int)ClientCmdEventType::TcpSendFileRequest_CurrentFile:{
            wstring filename = L"";
            uint64_t startpos = -1;
            m_cmdProcessor.getCurrRequestFileInfo(filename,startpos);
            appendInfo("continue downloading file :" + filename + " ,startpos = " + std::to_string(startpos));
            thread t1{[this,filename,startpos](){

                    unsigned char* buf = nullptr;
                    unsigned len = 0;
                    CmdFactory::makeSendFileRequest(filename,startpos,buf,len);//根据startpos获取文件部分数据
                    m_tcpClient.send((const char*)buf,len);
                    free(buf);
            }};//end of thread;
            t1.detach();

            }//end of case;
            break;
        case (int)ClientCmdEventType::TcpExecuteTaskRequest:
            string taskname = m_cmdProcessor.currTaskname();
            appendInfo("doing task :" + taskname);
            break;
        }//end of switch


        return;
    }

}

void MainFrame::appendInfo(const wxString& info)
{
    m_txtInfo->AppendText(info+ "\r\n");
    SetStatusText(info);
}
