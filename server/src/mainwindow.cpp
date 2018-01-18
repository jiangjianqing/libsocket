#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "CmdFactory.h"
#include "protos/udp_msg.discover.pb.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
}

MainWindow::~MainWindow()
{
    delete ui;
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

    //return msg.SerializeToArray(buf,len);
    //if(CmdFactory::makeDiscoverMsg("192.168.18.3",11211,buf,len)){
        //m_udpBroadcaster.broadcast(buf,len);//注意内存泄露
        //string str = CmdFactory::buf2Str(buf,len);
        /*
        fstream output("./cmd_log", ios::out | ios::trunc | ios::binary);
        output<<str;
        output.flush();*/
    //}

}

void MainWindow::on_btnStart_clicked()
{
    //m_tcpServer.start("0.0.0.0",11211);
}
