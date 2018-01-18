#include "MsgEventThread.h"

MsgEventThread::MsgEventThread(QObject *parent) :
    QThread(parent)
{
}

void MsgEventThread::run()
{
    //触发信号
    Msg msg;
    msg.int_info = 999;
    msg.str_info = "helloworld";
    emit TestSignal(msg);
}
