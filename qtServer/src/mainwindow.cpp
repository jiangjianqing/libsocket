#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MyEvent.h"
#include <thread>
#include <chrono>

//#include "CmdFactory.h"
//#include "udp_msg.discover.pb.h"

//总结：
//1、sendEvent只能用在同一个线程中；
//2、postEvent发送出去的QEvent对象会自动析构，无需释放。
void Thread1(void *lpParam)
{
    MainWindow* pWnd = (MainWindow*)lpParam;
    MyEvent me(QEvent::Type(QEvent::User+1));
    for ( int i=0; i<=10; ++i )
    {
        me.SetValue(i);
        //QApplication::sendEvent(pWnd, new MyEvent(me));
        /*
        QT的sendEvent和Win32里面的SendMessage一样呢，发送消息到窗口线程的消息循环，等待消息处理完毕后才返回。实践证明，不是那样的。

          最后，全部换成postEvent，并且QEvent对象都是通过new出来的。在消息循环中，我本来是用delete来释放这些申请的指针的，结果也是导致了崩溃了。查看调用堆栈，可以看到指针已经被QT内部机制析构了。代码通过开启两个线程，往消息循环中发送两个消息通知界面上的两个进度条不断改变进度。
        */
        QApplication::postEvent(pWnd, new MyEvent(me));
        //if ( i == 100 )
        //    i = 0;
        std::this_thread::sleep_for(chrono::milliseconds(10));
    }
    //return 0;
}



void Thread2(void *lpParam)
{
    MainWindow* pWnd = (MainWindow*)lpParam;
    MyEvent me(QEvent::Type(QEvent::User+2));
    int i = 10;
    while( i>0 )
    {
        me.SetValue(i);
        QApplication::postEvent(pWnd, new MyEvent(me));
        //if ( i == 0 )
        //    i = 100;
        std::this_thread::sleep_for(chrono::milliseconds(10));
        i--;
    }
    //return 0;
}

int call_time = 0;

void CloseCB(int clientid, void* userdata)
{
    //fprintf(stdout, "cliend %d close\n", clientid);
    //uv::TCPClient* client = (uv::TCPClient*)userdata;
    //client->Close();
}

void ReadCB(const unsigned char* buf,const unsigned len, void* userdata)
{
    fprintf(stdout,"call time %d\n",++call_time);
    if (call_time > 5000) {
        return;
    }
    char senddata[256] = {0};
    //uv::TCPClient* client = (uv::TCPClient*)userdata;
    //sprintf(senddata, "****recv server data(%p,%d)", client, packet.datalen);
    fprintf(stdout, "%s\n", senddata);
    //NetPacket tmppack = packet;
    //tmppack.datalen = (std::min)(strlen(senddata), sizeof(senddata) - 1);
    //std::string retstr = PacketData(tmppack, (const unsigned char*)senddata);
    /*
    if (client->Send(&retstr[0], retstr.length()) <= 0) {
        fprintf(stdout, "(%p)send error.%s\n", client, client->GetLastErrMsg());
    }
    */
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
    int i = 0;
    i++;
}

void TcpServer_NewConnect(int clientid, void* userdata)
{
    fprintf(stdout,"new connect:%d\n",clientid);
    uv::TCPServer *theclass = (uv::TCPServer *)userdata;
    theclass->setRecvCB(clientid,TcpServer_ReadCB,userdata);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*2018.01.18 特别重要：
     * 在线程间使用信号槽进行通信时，需要注意必须使用元数据类型
    Qt内生的元数据类型，如int double QString 等
    如果要用自己定义的数据类型，需要在connect前将其注册为元数据类型。形式见代码。
    */
    //线程通信 方案1： sign & slot
    qRegisterMetaType<Msg>("Msg");
    //进行connect前必须实例化
    t = new MsgEventThread();
    connect(t, &MsgEventThread::TestSignal, this, &MainWindow::DisplayMsg);

    //执行子线程
    t->start();

    std::thread t1{Thread1,this};
    t1.detach();

    std::thread t2{Thread2,this};
    t2.detach();

    /*
    m_udpBroadcaster.start(8899);

    auto tcpCb= [this](AbstractSocket* socket,const socket_event_t& event,const char* buf, int nread,socket_reply_cb reply){
        switch(event.type){
        case socket_event_type::ConnectionAccept:
            onClientAccepted(event);
            break;
        case socket_event_type::ConnectionClose:
            onClientClosed(event);
            break;
        case socket_event_type::ReadData:
            break;
        default:
            onSimpleSocketEvent(event.type);
            break;
        }
    };

    m_tcpServer.setSocketEventCb(tcpCb);
    */
    /*
    m_tcpClient.setRecvCB(ReadCB, &m_tcpClient);
    m_tcpClient.setClosedCB(CloseCB, &m_tcpClient);
    uv::TcpClient::StartLog("log/");
    */

    uv::TCPServer::StartLog("log/");
    m_tcpServer.setNewConnectCB(TcpServer_NewConnect,&m_tcpServer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DisplayMsg(Msg msg)
{
    //ui->textBrowser->append(QString::number(a));
    ui->textBrowser->append(QString::number(msg.int_info));
    ui->textBrowser->append(QString::fromStdString(msg.str_info));
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::User+1:
    {
        MyEvent* pEvent = (MyEvent*)event;
        ui->textBrowser->append(QString("User+1  event"));
        //ui->progressBar->setValue(pEvent->GetValue());
        //delete pEvent;
        return 0;
    }
        break;
    case QEvent::User+2:
    {
        MyEvent* pEvent = (MyEvent*)event;
        ui->textBrowser->append(QString("User+2  event"));
        //ui->progressBar_2->setValue(pEvent->GetValue());
        //delete pEvent;
        return 0;
    }
        break;
    default:
        break;
    }
    return QMainWindow::event(event);
}

/*
void MainWindow::onClientAccepted(const socket_event_t& event)
{
    string label = event.ip + ":" + std::to_string(event.port);
    QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(label),ui->listUpdater);
    ui->listUpdater->addItem(item);
}

void MainWindow::onClientClosed(const socket_event_t& event)
{

}

void MainWindow::onSimpleSocketEvent(socket_event_type type)
{

}
*/

void MainWindow::on_pushButton_clicked()
{
    char* buf = nullptr;
    int len = 0;

    //m_tcpClient.close();
    m_tcpServer.close();
}

void MainWindow::on_btnStart_clicked()
{
/*
    if (!m_tcpClient.connect("192.168.18.29",11212)) {
        fprintf(stdout, "connect error:%s\n", m_tcpClient.getLastErrMsg());
    } else {
        fprintf(stdout, "client(%p) connect succeed.\n", &m_tcpClient);
    }
    */

    if(!m_tcpServer.start("0.0.0.0",12345)) {
            fprintf(stdout,"Start Server error:%s\n",m_tcpServer.getLastErrMsg());
    }
    m_tcpServer.setKeepAlive(1,60);//enable Keepalive, 60s

    fprintf(stdout,"server return on main.\n");

}
