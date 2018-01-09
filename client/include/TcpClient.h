#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "AbstractSocket.h"

class TcpClient : public AbstractSocket
{
public:
    explicit TcpClient();
    virtual ~TcpClient();

    //bool connect(const string &ip, int port) override;
    void close() override;
};

#endif // TCPCLIENT_H
