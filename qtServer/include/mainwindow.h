#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "UdpBroadcaster.h"
//#include "TcpServer.h"
#include <string>
#include "MsgEventThread.h"
#include "Msg.h"
//#include "tcpclient.h"
#include "tcpserver.h"
#include "UdpBroadcaster.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_btnStart_clicked();

    void on_btnSend_clicked();

protected:
    bool event(QEvent *event);//用QEvent方式完成线程通信

private:
    Ui::MainWindow *ui;

    MsgEventThread *t;

    void DisplayMsg(Msg);

    //uv::TcpClient m_tcpClient;
    uv::TcpServer m_tcpServer;
    uv::UdpClient m_udpBroadcaster;

    //UdpBroadcaster m_udpBroadcaster;
    //TcpServer m_tcpServer;

    //void onClientAccepted(const socket_event_t& event);
    //void onClientClosed(const socket_event_t& event);
    //void onSimpleSocketEvent(socket_event_type type);
};

#endif // MAINWINDOW_H
