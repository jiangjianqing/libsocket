#include "MainFrame.h"

#include <string>
#include "wx/filedlg.h"
#include "CmdFactory.h"
#include "wx/msgdlg.h"
#include <sstream>
#include "CmdProcesserThreadEvent.h"
#include "commonutils/FileUtils.h"
#include <thread>

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__
#endif

#ifndef BUILD_TIME
#define BUILD_TIME __TIME__
#endif

const string kTitle = string("wxServer (  ") + string(BUILD_DATE) + "  )";
const int kFilePartSize = 60*1024;//文件分包大小

const unsigned short kTcpServerPort = 11212;

#if defined(__linux__)
// Linux系统
static const string kFileRepoPath = "/home/cz_jjq/git/cpp/libsocket/qtServer";
#elif defined(_WIN32)
// Windows系统
static const string kFileRepoPath = "D:\\git\\cpp\\libsocket\\qtServer";
#endif


MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, kTitle) , m_isRunning(false)
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
    m_btnBroadcast = new wxButton(m_bottomPanel,ID_BTN_BROADCAST,wxT("broadcast"));
    m_btnStart = new wxButton(m_bottomPanel,ID_BTN_START,wxT("start"));
    m_btnSendFile = new wxButton(m_bottomPanel,ID_BTN_SENDFILE,wxT("sendfile"));

    m_bottomSizer->Add(m_txtFilename, 1, wxEXPAND);
    m_bottomSizer->Add(m_btnSelectFile,0,wxLEFT);
    m_bottomSizer->Add(m_btnSendFile,0,wxLEFT);
    m_bottomSizer->Add(m_btnBroadcast, 0, wxRIGHT);
    m_bottomSizer->Add(m_btnStart, 0,wxRIGHT, 5);


    Bind(wxEVT_BUTTON,&MainFrame::onBtnSelectFileClick,this,ID_BTN_SELECTFILE);
    Bind(wxEVT_BUTTON,&MainFrame::onBtnStartClick,this,ID_BTN_START);
    Bind(wxEVT_BUTTON,&MainFrame::onBtnBroadcastClick,this,ID_BTN_BROADCAST);
    Bind(wxEVT_BUTTON,&MainFrame::onBtnSendFileClick,this,ID_BTN_SENDFILE);

    initSocket();

    m_isRunning = true;//故意设置为true，通过调用onBtnStartClick来设置初始化状态
    wxCommandEvent e;
    onBtnStartClick(e);
}

MainFrame::~MainFrame()
{
    for(auto it = m_cmdProcessers.begin();it != m_cmdProcessers.end(); it++){
        delete it->second;
    }
}

void TcpServer_CloseCB(int clientid, void* userdata)
{
    fprintf(stdout,"cliend %d close\n",clientid);
    //uv::TCPServer *theclass = (uv::TCPServer *)userdata;
    //theclass->Close();
    //is_eist = true;
}

void TcpServer_ReadCB(int clientid, const unsigned char* buf,const unsigned len, void* userdata)
{
    MainFrame *mainFrame = (MainFrame *)userdata;
    auto it = mainFrame->m_cmdProcessers.find(clientid);
    if(it != mainFrame->m_cmdProcessers.end()){
        ServerCmdProcesser* cmdProcesser = it->second;
        cmdProcesser->recvData((const char*)buf,len);
    }
    //mainFrame->m_cmdProcessers[clientid]->recvTcpData((const char*)buf,len);
}

void TcpServer_NewConnect(int clientid, void* userdata)
{
    fprintf(stdout,"new connect:%d\n",clientid);
    MainFrame *mainFrame = (MainFrame *)userdata;
    ServerCmdProcesser* cmdProcesser = new ServerCmdProcesser(mainFrame,clientid);
    mainFrame->m_cmdProcessers.insert(make_pair(clientid,cmdProcesser));
    mainFrame->m_tcpServer.setRecvCB(clientid,TcpServer_ReadCB,mainFrame);
    unsigned char* buf = nullptr;
    unsigned len = 0;
    CmdFactory::makeIdentifyRequestMsg(buf,len);
    mainFrame->m_tcpServer.send((const char*)buf,len,clientid);
    free(buf);
}

void MainFrame::initSocket()
{
    Bind(wxEVT_THREAD,&MainFrame::onSocketThreadEvent,this);

    m_tcpServer.setKeepAlive(1,60);//enable Keepalive, 60s
    m_tcpServer.setNewConnectCB(TcpServer_NewConnect,this);
}


void MainFrame::onSocketThreadEvent(wxThreadEvent& event)
{
    if(typeid(event) == typeid(CmdProcesserThreadEvent)){
        CmdProcesserThreadEvent& cmdProcessEvent = (CmdProcesserThreadEvent&)event;
        ServerCmdProcesser* cmdProcesser = cmdProcessEvent.cmdProcesser();
        int clientId = cmdProcesser->clientId();
        switch(event.GetId()){
        case (int)ServerCmdEventType::TcpIdentifyResponse:
        {
            string temp = "client online , clientIdd = " + std::to_string(clientId) + " , identifyId = " + std::to_string(cmdProcesser->identifyResponseId());
            appendInfo(temp);
            //发送更新请求
            unsigned char* buf = nullptr;
            unsigned len = 0;
            CmdFactory::makeStartUpdateRequestMsg(buf,len);
            m_tcpServer.send((const char*)buf,len,clientId);
            free(buf);
        }
            break;
        case (int)ServerCmdEventType::TcpFileListResponse:{
            thread t1{[this,clientId](){
                    vector<string> relativeFilenameList = FileUtils::ls_R(kFileRepoPath,true,[](const string& filename){
                        return true;
                    });
                    //生成文件摘要信息列表，这里使用智能指针更好
                    vector<file_brief_info_t*> files;
                    for(auto it = relativeFilenameList.begin(); it != relativeFilenameList.end(); it++){
                        string relativeFilename = *it;
                        string fullFilename = kFileRepoPath + relativeFilename;
                        file_brief_info_t* info = CmdFactory::generateFileBriefInfo(fullFilename);
                        if(info != nullptr){
                            strcpy(info->filename,relativeFilename.data());
                            files.push_back(info);
                        }
                    }
                    unsigned char* buf = nullptr;
                    unsigned len = 0;
                    CmdFactory::makeFileListResponse(files,buf,len);
                    m_tcpServer.send((const char*)buf,len,clientId);
                    free(buf);
                    //释放文件摘要资源
                    for(auto it = files.begin(); it != files.end(); it++){
                        file_brief_info_t* info = *it;
                        free(info);
                    }
            }};
            t1.detach();
            }
            break;
        case (int)ServerCmdEventType::TcpSendFileResponse:{
            thread t1{[this,cmdProcesser,clientId](){
                string relativeFilename = cmdProcesser->currRequestFilename();
                uint64_t start_pos = cmdProcesser->currRequestFileStartPos();
                string fullFilename = kFileRepoPath + relativeFilename;

                unsigned char* file_data = (unsigned char*)malloc(kFilePartSize);//按60k的包大小发送文件
                unsigned file_size = FileUtils::getFileSize(fullFilename);

                unsigned char* buf = nullptr;//注意：任何情况下都需要给客户端一个回应


                int read_count = FileUtils::ReadFileData(fullFilename,start_pos,(char*)file_data,kFilePartSize);

                if(read_count >= 0){
                    unsigned len = 0;
                    uint64_t residue_length = file_size  - start_pos - read_count;
                    if(residue_length > 0){
                        int i = 0;
                        i = i + 1;
                    }
                    CmdFactory::makeSendFileResponseMsg(relativeFilename,(const char*)file_data,read_count,start_pos,residue_length,buf,len);
                    m_tcpServer.send((const char*)buf,len,clientId);
                }else{//让异常状态给客户端，让其进行标记,并开始获取下一个文件
                    assert("没有读到文件");
                }
                free(buf);//注意：任何情况下都需要给客户端一个回应
                free(file_data);
            }};
            t1.detach();
            }
            break;
        case (int)ServerCmdEventType::TcpAllFilesSendResult:{
            thread t1{[this,cmdProcesser,clientId](){
                if(cmdProcesser->isAllFilesSendOk()){
                    unsigned char* buf = nullptr;//注意：任何情况下都需要给客户端一个回应
                    unsigned len = 0;
                    CmdFactory::makeExecuteTaskRequest("upgrade",buf,len);
                    m_tcpServer.send((const char*)buf,len,clientId);
                    free(buf);
                }else{//isAllFilesSendOk() == false
                    //assert("文件接收不完整");
                }
            }};
            t1.detach();
            }
            break;
        }//end of switch
        return;
    }
    wxString msg = event.GetString();
    string info = "uninitialized info!";

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
    m_txtInfo->AppendText(info+ "\r\n");
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

void MainFrame::onBtnSendFileClick(wxCommandEvent& event)
{
    string filename = m_txtFilename->GetLineText(0).ToStdString();



}

void MainFrame::onBtnBroadcastClick(wxCommandEvent& event)
{
    if(m_isRunning){
        char* buf = nullptr;
        int len = 0;
        if(CmdFactory::makeDiscoverMsg("127.0.0.1",kTcpServerPort,buf,len)){
            m_udpBroadcaster.send(buf,len);

            free(buf);
            /*
            string str = CmdFactory::buf2Str(buf,len);
            fstream output("./cmd_log", ios::out | ios::trunc | ios::binary);

            output<<str;
            output.flush();
            output.close();
            cmdProcesser.protobuf_test("udp_msg.discover",buf,len);
            */
        }
    }else{
        wxMessageDialog dlg(nullptr,wxT("please start server first"));
        dlg.ShowModal();
    }
}

void MainFrame::onBtnStartClick(wxCommandEvent& event)
{
    if(m_isRunning){
        m_tcpServer.close();
        m_udpBroadcaster.close();
        m_btnBroadcast->Disable();
        m_btnStart->SetLabel(wxT("start"));
    }else{
        m_txtInfo->Clear();//清除文本框
        m_udpBroadcaster.connect(nullptr,8899);
        m_tcpServer.start("0.0.0.0",kTcpServerPort);
        m_btnBroadcast->Enable();
        m_btnStart->SetLabel(wxT("stop"));
    }
    m_isRunning = !m_isRunning;
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
