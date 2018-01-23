#ifndef UDPBROADCASTER_H
#define UDPBROADCASTER_H
#include "uv.h"
#include "net_base.h"
#include <list>
#include "pod_circularbuffer.h"
#ifndef BUFFER_SIZE
#define BUFFER_SIZE (1024*10)
#endif

namespace uv
{

using namespace  std;
class UdpClient
{
public:
    UdpClient();
    virtual ~UdpClient();
    //Start/Stop the log
    static void StartLog(const char* logpath = nullptr);
    static void StopLog();
public:
    void setRecvCB(ClientRecvCB pfun, void* userdata);//set recv cb
    void setClosedCB(TcpCloseCB pfun, void* userdata);//set close cb.
    bool connect(const char* ip, int port);//connect the server, ipv4
    bool connect6(const char* ip, int port);//connect the server, ipv6
    int  send(const char* data, std::size_t len);//send data to server
    void close();//send close command. verify IsClosed for real closed
    bool isClosed() {//verify if real closed
        return isclosed_;
    };

    const char* getLastErrMsg() const {
        return errmsg_.c_str();
    };

protected:
    bool init();
    void closeinl();//real close fun
    bool run(int status = UV_RUN_DEFAULT);
    void send_inl(uv_udp_send_t* req = NULL);//real send data fun
    static void ConnectThread(void* arg);//connect thread,run until use close the client

    static void AfterRecv(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
    static void AfterSend(uv_udp_send_t* req, int status);
    static void AllocBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
    static void AfterClientClose(uv_handle_t* handle);
    static void AsyncCB(uv_async_t* handle);//async close
    static void CloseWalkCB(uv_handle_t* handle, void* arg);//close all handle in loop
    static void GetPacket(const unsigned char* packetdata,const unsigned packetlen, void* userdata);

private:
    enum {
        CONNECT_TIMEOUT,
        CONNECT_FINISH,
        CONNECT_ERROR,
        CONNECT_DIS,
    };
    bool isBroadcast_;

    UdpClientCtx *client_handle_;
    uv_async_t async_handle_;
    uv_loop_t loop_;
    bool isclosed_;
    bool isuseraskforclosed_;

    uv_thread_t connect_threadhandle_;
    uv_connect_t connect_req_;

    int connectstatus_;

    //send param
    uv_mutex_t mutex_writebuf_;//mutex of writebuf_list_
    list<udp_send_param*> writeparam_list_;//Availa write_t
    PodCircularBuffer<char> write_circularbuf_;//the data prepare to send

    ClientRecvCB recvcb_;
    void* recvcb_userdata_;

    TcpCloseCB closedcb_;
    void* closedcb_userdata_;

    //uv_timer_t reconnect_timer_;

    std::string connectip_;
    int connectport_;
    bool isIPv6_;
    std::string errmsg_;

};

}//end of namespace uv

#endif // UDPBROADCASTER_H
