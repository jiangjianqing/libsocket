#include "MainFrame.h"

#include <string>
#include "wx/filedlg.h"
#include "CmdFactory.h"
#include "wx/msgdlg.h"
#include <sstream>

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__
#endif

#ifndef BUILD_TIME
#define BUILD_TIME __TIME__
#endif

const string kTitle = string("wxServer (  ") + string(BUILD_DATE) + "  )";

const unsigned short kTcpServerPort = 11212;

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, kTitle)
{
    //Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);//c++11 lambda
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    //Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MyFrame::OnClose));//Connect is deprecated!!!!!
    Bind(wxEVT_CLOSE_WINDOW,&MainFrame::OnClose,this);

    CreateStatusBar();
    //创建最低层面板,垂直布局
    //wxPanel *topPanel=new wxPanel(this);
    m_mainSizer=new wxBoxSizer(wxVERTICAL);//垂直布局，可用wxHORIZONTAL水平布局替换。
    //topPanel->SetSizer(topSizer);
    this->SetSizer(m_mainSizer);
    //创建上中下三个面板,上下面板高度固定,宽度自由伸展,中间面板上下左右四个方向自由伸展
    m_topPanel=new wxPanel(this);
    m_topPanel->SetBackgroundColour(*wxBLACK);//黑色背景
    m_centerPanel=new wxPanel(this);
    m_centerPanel->SetBackgroundColour(*wxWHITE);//白色背景
    m_bottomPanel=new wxPanel(this);
    m_bottomPanel->SetBackgroundColour(*wxRED);//红色背景

    m_txtInfo = new wxTextCtrl(m_centerPanel,ID_TEXT_INFO,"",wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL);


    m_mainSizer->Add(m_topPanel,0,wxEXPAND);
    m_mainSizer->Add(m_centerPanel,
            1,                   //垂直方向可拉伸
            wxEXPAND|            //水平方向自动填充
            wxALL,               //四周都有边框
            10                   //边框宽度为10
            );
    m_mainSizer->Add(m_bottomPanel,0,wxEXPAND);

    m_centerSizer = new wxBoxSizer(wxVERTICAL);
    m_centerSizer->Add(m_txtInfo,
                       1,                   //垂直方向可拉伸
                       wxEXPAND|            //水平方向自动填充
                       wxLEFT|wxBOTTOM|wxRIGHT,               //四周都有边框
                      0                   //边框宽度为0
    );
    m_centerPanel->SetSizer(m_centerSizer);

    m_bottomSizer = new wxBoxSizer(wxHORIZONTAL);
    m_bottomPanel->SetSizer(m_bottomSizer);


    m_txtFilename =new wxTextCtrl(m_bottomPanel,ID_TEXT_FILENAME,"",wxDefaultPosition,wxDefaultSize,wxTE_READONLY);
    m_btnSelectFile = new wxButton(m_bottomPanel,ID_BTN_SELECTFILE,"...",wxDefaultPosition,wxSize(30,28));
    m_btnTest = new wxButton(m_bottomPanel,ID_BTN_TEST,wxT("test"));
    m_btnStart = new wxButton(m_bottomPanel,ID_BTN_START,wxT("start"));

    m_bottomSizer->Add(m_txtFilename, 1, wxEXPAND);
    m_bottomSizer->Add(m_btnSelectFile,0,wxLEFT);
    m_bottomSizer->Add(m_btnTest, 0, wxLEFT);
    m_bottomSizer->Add(m_btnStart, 0,wxLEFT, 5);


    Bind(wxEVT_BUTTON,&MainFrame::onBtnSelectFileClick,this,ID_BTN_SELECTFILE);
    Bind(wxEVT_BUTTON,&MainFrame::onBtnStartClick,this,ID_BTN_START);

    initSocket();
}

void MainFrame::initSocket()
{
    Bind(wxEVT_THREAD,&MainFrame::onSocketThreadEvent,this);

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

    m_udpBroadcaster.start(8899);
    m_tcpServer.setSocketEventCb(tcpCb);
    m_tcpServer.start("0.0.0.0",kTcpServerPort);
}

void MainFrame::onClientAccepted(const string& ip,int port)
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

void MainFrame::onClientClosed(int id,const string& ip,int port)
{
    wxThreadEvent event(wxEVT_COMMAND_THREAD, wxID_ANY);
    event.SetId((int)socket_event_type::ConnectionClose);
    event.SetString(std::to_string(id) + ":" + ip + ":" + std::to_string(port));
    wxQueueEvent(this,event.Clone());
}


void MainFrame::onSimpleTcpSocketEvent(socket_event_type type)
{
    wxThreadEvent event(wxEVT_COMMAND_THREAD, wxID_ANY);
    event.SetId((int)type);
    //event.SetString(std::to_string(id) + ":" + ip + ":" + std::to_string(port));
    wxQueueEvent(this,event.Clone());
}

void MainFrame::onSocketThreadEvent(wxThreadEvent& event)
{
    wxString msg = event.GetString();
    string info = "uninitialized info!";
    switch(event.GetId()){
    case (int)socket_event_type::ConnectionAccept:
        info = "client accepted : " + msg;
        break;
    case (int)socket_event_type::ConnectFault:
        info = "client connect fault : ";
        break;
    case (int)socket_event_type::ConnectionClose:
        info = "client closed : " + msg;
        break;
    case (int)socket_event_type::Listening:
        info = "server is listening !";
        break;
    case (int)socket_event_type::ListenFault:
        info = "establish listen fault !";
        break;
    case (int)socket_event_type::Unknow:
        info = "occur unknow socket event !";
        break;
    case (int)socket_event_type::SocketClose:
        info = "socket closed !";
        break;
    }
    appendInfo(info);
/*
 *
 * //另一个thread中发送消息
    wxThreadEvent e(wxEVT_COMMAND_THREAD, ID_MY_THREAD_EVENT);
    e.SetString(_T("Some string"));
    wxTheApp->QueueEvent(e.Clone());
  */


}

void MainFrame::appendInfo(const string& info)
{
    m_txtInfo->AppendText(info);
    SetStatusText(info);
}

void MainFrame::onBtnSelectFileClick(wxCommandEvent& event)
{
    wxFileDialog dlg(this,wxT("打开文件"),wxT(""),wxT(""),
        wxT("所有文件(*)|*|C++源程序(*.cpp)|*.cpp"),
        wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if(dlg.ShowModal()==wxID_CANCEL)
        return;
    m_txtFilename->SetValue(dlg.GetPath());
    //m_txtInfo->LoadFile(dlg.GetPath());
}

void MainFrame::onBtnStartClick(wxCommandEvent& event)
{
    char* buf = nullptr;
    int len = 0;
    CmdFactory::makeDiscoverMsg("127.0.0.1",kTcpServerPort,buf,len);
    m_udpBroadcaster.broadcast(buf,len);
    free(buf);
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