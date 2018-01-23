/***************************************
* @file     packet_sync.h
* @brief    TCP 数据包封装.依赖libuv,openssl.功能：接收数据，解析得到一帧后回调给用户。同步处理，接收到马上解析
* @details  根据net_base.h中NetPacket的定义，对数据包进行封装。
			md5校验码使用openssl函数
			同一线程中实时解码
			长度为0的md5为：d41d8cd98f00b204e9800998ecf8427e，改为全0. 编解码时修改。
//调用方法
Packet packet;
packet.SetPacketCB(GetPacket,&serpac);
packet.Start(0x01,0x02);
//socket有数据到达时，调用packet.recvdata((const unsigned char*)buf,bufsize); 只要足够一帧它会触发GetFullPacket

* @author   phata, wqvbjhc@gmail.com
* @date     2014-05-21
* @mod      2014-08-04 phata 修复解析一帧数据有误的bug
            2014-11-12 phata GetUVError冲突，改为使用thread_uv.h中的
****************************************/
#ifndef PACKET_SYNC_H
#define PACKET_SYNC_H
#include <algorithm>
#include <string.h>
#include "thread_uv.h"//for GetUVError
#if defined (WIN32) || defined(_WIN32)
#include <windows.h>
#define ThreadSleep(ms) Sleep(ms);//睡眠ms毫秒
#elif defined __linux__
#include <unistd.h>
#define ThreadSleep(ms) usleep((ms) * 1000)//睡眠ms毫秒
#endif

typedef void (*GetFullPacket)(const unsigned char* packetdata,const unsigned packetlen, void* userdata);

#ifndef BUFFER_SIZE
#define BUFFER_SIZE (1024*10)
#endif
class PacketSync;

typedef struct _tcpclient_ctx {
    uv_tcp_t tcphandle;//store this on data
    uv_write_t write_req;//store this on data
    PacketSync* packet_;//store this on userdata
    uv_buf_t read_buf_;
    int clientid;
    void* parent_server;//store TCPClient point
    void* parent_acceptclient;//accept client
} TcpClientCtx;
TcpClientCtx* AllocTcpClientCtx(void* parentserver);
void FreeTcpClientCtx(TcpClientCtx* ctx);

typedef struct _udpclient_ctx {
    uv_udp_t udphandle;//store this on data
    uv_write_t write_req;//store this on data
    PacketSync* packet_;//store this on userdata
    uv_buf_t read_buf_;
    //int clientid;
    void* parent_server;//store UdpClient point
} UdpClientCtx;
UdpClientCtx* AllocUdpClientCtx(void* parentserver);
void FreeUdpClientCtx(UdpClientCtx* ctx);

typedef struct _write_param{//param of uv_write
    uv_write_t write_req_;//store TCPClient on data
    uv_buf_t buf_;
    int buf_truelen_;
}write_param;
write_param * AllocWriteParam(void);
void FreeWriteParam(write_param* param);

typedef struct _udp_send_param{//param of uv_udp_send
    uv_udp_send_t write_req_;//store TCPClient on data
    uv_buf_t buf_;
    int buf_truelen_;
}udp_send_param;
udp_send_param * AllocUdpSendParam(void);
void FreeUdpSendParam(udp_send_param* param);

class PacketSync
{
public:
    PacketSync(): packet_cb_(NULL), packetcb_userdata_(NULL) {
        thread_readdata = uv_buf_init((char*)malloc(BUFFER_SIZE), BUFFER_SIZE); //负责从circulebuffer_读取数据
        thread_packetdata = uv_buf_init((char*)malloc(BUFFER_SIZE), BUFFER_SIZE); //负责从circulebuffer_读取packet 中data部分
    }
    virtual ~PacketSync() {
        free(thread_readdata.base);
        free(thread_packetdata.base);
    }

    /*
    bool Start() {
        return true;
    }
    */

public:
    void recvdata(const unsigned char* data, int len) { //接收到数据，把数据保存在circulebuffer_
        //回调帧数据给用户
        if (this->packet_cb_) {
            memcpy(thread_packetdata.base,data,len);
            this->packet_cb_((const unsigned char*)thread_packetdata.base,len, this->packetcb_userdata_);
        }
    }
    void SetPacketCB(GetFullPacket pfun, void* userdata) {
        packet_cb_ = pfun;
        packetcb_userdata_ = userdata;
    }
private:

    GetFullPacket packet_cb_;//回调函数
    void*         packetcb_userdata_;//回调函数所带的自定义数据

    uv_buf_t  thread_readdata;//负责从circulebuffer_读取数据
    uv_buf_t  thread_packetdata;//负责从circulebuffer_读取packet 中data部分
private:// no copy
    PacketSync(const PacketSync&);
    PacketSync& operator = (const PacketSync&);
};


//客户端或服务器关闭的回调函数
//服务器：当clientid为-1时，表现服务器的关闭事件
//客户端：clientid无效，永远为-1
typedef void (*TcpCloseCB)(int clientid, void* userdata);

//TCPServer接收到新客户端回调给用户
typedef void (*NewConnectCB)(int clientid, void* userdata);

//TCPServer接收到客户端数据回调给用户
typedef void (*ServerRecvCB)(int clientid, const unsigned char* buf,const unsigned len, void* userdata);

//TCPClient接收到服务器数据回调给用户
typedef void (*ClientRecvCB)(const unsigned char* buf,const unsigned len, void* userdata);

//网络事件类型
typedef enum {
    NET_EVENT_TYPE_RECONNECT = 0,  //与服务器自动重连成功事件
    NET_EVENT_TYPE_DISCONNECT      //与服务器断开事件
} NET_EVENT_TYPE;
//TCPClient断线重连函数
typedef void (*ReconnectCB)(NET_EVENT_TYPE eventtype, void* userdata);

#endif//PACKET_SYNC_H
