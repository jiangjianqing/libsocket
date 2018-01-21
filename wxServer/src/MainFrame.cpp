#include "MainFrame.h"

#include <string>
#include "wx/filedlg.h"
#include "CmdFactory.h"

using namespace std;

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__
#endif

#ifndef BUILD_TIME
#define BUILD_TIME __TIME__
#endif

const string kTitle = string("wxServer (  ") + string(BUILD_DATE) + "  )";

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, kTitle)
{
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

    m_udpBroadcaster.start(8899);
    m_tcpServer.start("0.0.0.0",11212);

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
    CmdFactory::makeDiscoverMsg("127.0.0.1",8899,buf,len);
    m_udpBroadcaster.broadcast(buf,len);
    free(buf);
}
