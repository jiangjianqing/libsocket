#ifndef MSGEVENTTHREAD_H
#define MSGEVENTTHREAD_H

#include <QThread>
#include "Msg.h"

class MsgEventThread : public QThread
{
    Q_OBJECT
public:
    explicit MsgEventThread(QObject *parent = 0);

protected:
        void run();

signals:
    void TestSignal(Msg);

private:
    //Msg msg;
};

#endif // MSGEVENTTHREAD_H
