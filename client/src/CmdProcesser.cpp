#include "CmdProcesser.h"
#include "SocketData.h"
#include "CmdFactory.h"
#include "protos/udp_msg.discover.pb.h"

CmdProcesser::CmdProcesser()
{

}

void CmdProcesser::recvUdpData(const char* buf,int nread,socket_reply_cb replyCb)
{
    //只识别一个discover命令,而且为可见字符集
    udp_msg::discover msg;
    if(msg.ParseFromArray(buf,nread)){
        //收到discover信息
        string ip = msg.server_ip();
        int port = msg.server_port();
    }

}
