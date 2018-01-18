#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UdpBroadcaster.h"
#include "TcpServer.h"
#include <string>

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

private:
    Ui::MainWindow *ui;

    UdpBroadcaster m_udpBroadcaster;
    TcpServer m_tcpServer;

    void onClientAccepted(const socket_event_t& event);
    void onClientClosed(const socket_event_t& event);
    void onSimpleSocketEvent(socket_event_type type);
};

#endif // MAINWINDOW_H
