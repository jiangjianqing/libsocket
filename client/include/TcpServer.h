#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "AbstractSocket.h"

class TcpServer : public AbstractSocket
{
public:
    explicit TcpServer();
    virtual ~TcpServer();

    bool connect(const string &ip, int port) override;
    void close() override;

private:
    bool bind(const string& ip, int port);
    bool bind6(const string& ip, int port);
    bool listen(int backlog);
    int getAvailableClientId() const{static int s_id = 0; return ++s_id; }
};

#endif // TCPSERVER_H
