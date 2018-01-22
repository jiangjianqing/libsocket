//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include <stdio.h>
//#include <QThread>
#include "IoThread.h"
#include <thread>


static void ThreadFuntion(void* arg);
static void Udp_onAllocate(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
static void Udp_onReceived(uv_udp_t* handle, ssize_t nread, const uv_buf_t* pBuf,
                           const struct sockaddr* addr, unsigned flags);
static void IoThread_onSent(uv_udp_send_t* req, int status);

IoThread::IoThread()
{
    //List_Initialize(&_requests);
    //List_Initialize(&_pendingRequests);

    uv_mutex_init(&_mutex);
    uv_async_init(uv_default_loop(), &_uvaRequestNofity, onRequest);
    _uvaRequestNofity.data = this;

    uv_udp_init(uv_default_loop(), &_udp);
    _udp.data = this;
}

IoThread::~IoThread()
{
}

void IoThread::onRequest(uv_async_t* handle){
    //Qt::HANDLE h = QThread::currentThreadId();
    IoThread *pThread = (IoThread*)handle->data;
    pThread->ProcessRequests();
}

void IoThread::SetLocal(unsigned short nPort)
{
    struct sockaddr_in addr;
    uv_ip4_addr("127.0.0.1", nPort, &addr);
    uv_udp_bind(&_udp, (const struct sockaddr*) &addr, 0);
    uv_udp_recv_start(&_udp, Udp_onAllocate, Udp_onReceived);
}

void IoThread::SetRemote(const string &strIp, unsigned short nPort){
    uv_ip4_addr(strIp.c_str(), nPort, &_siRemote);
}

bool IoThread::PostUdp(char *pData, unsigned nBytes){
    //Qt::HANDLE h = QThread::currentThreadId();
    if(nBytes > MAX_UDP_DATA_SIZE){
        free(pData);
        return false;
    }

    IoRequest *pRequest = (IoRequest*)malloc(sizeof(IoRequest) + sizeof(uv_udp_send_t));
    pRequest->type = etPostUdp;
    pRequest->pData = pData;
    pRequest->nBytes = nBytes;

    return Post(pRequest);
}

bool IoThread::Post(IoRequest *pRequest){
    uv_mutex_lock(&_mutex);
    //List_Push(&_pendingRequests, (DoubleNode*)pRequest);
    uv_mutex_unlock(&_mutex);
    return uv_async_send(&_uvaRequestNofity) == 0;
}

void IoThread::ProcessRequests(){
    uv_mutex_lock(&_mutex);
    //List_MoveTo(&_pendingRequests, &_requests);
    uv_mutex_unlock(&_mutex);

    /*
    List *pRequests = &_requests;
    while(List_NotEmpty(pRequests)){
        IoRequest *pRequest = (IoRequest*)List_Pop(pRequests);
        ProcessRequest(pRequest);
    }*/
}

void IoThread::ProcessRequest(IoRequest *pRequest){
    switch(pRequest->type){
    case etPostUdp:{
        uv_udp_send_t *puvRequest = (uv_udp_send_t*)(pRequest + 1);
        uv_buf_t buf = uv_buf_init((char*)pRequest->pData, pRequest->nBytes);
        if(uv_udp_send(puvRequest, &_udp, &buf, 1, (const struct sockaddr*)&_siRemote, IoThread_onSent) != 0){
            free(pRequest->pData);
            free(pRequest);
        }
    }break;
    }
}



bool IoThread::Run(){
    return uv_thread_create(&_thread, ThreadFuntion, this) == 0;
}

void IoThread::stop()
{
    uv_stop(uv_default_loop());

}

static void Udp_onAllocate(uv_handle_t* handle,
                           size_t suggested_size,
                           uv_buf_t* buf) {
    char *slab = (char*)malloc(65536);
    buf->base = slab;
    buf->len = 65536;
}

static void Udp_onReceived(uv_udp_t* handle,
                           ssize_t nread,
                           const uv_buf_t* pBuf,
                           const struct sockaddr* addr,
                           unsigned flags){
    //Qt::HANDLE h = QThread::currentThreadId();
    if(nread == 0){
        free(pBuf->base);
        return;
    }

    if (nread < 0) {
        free(pBuf->base);
        return;
    }

    IoThread *pThread = (IoThread*)handle->data;
    //emit pThread->receivedUdp((Byte*)pBuf->base, nread);
    //    free(pBuf->base);
}

static void IoThread_onSent(uv_udp_send_t* req, int status){
    //Qt::HANDLE h = QThread::currentThreadId();
    int r;
    if(status != 0){
        printf("Client_onSent\n");
    }

    IoRequest *pRequest = (IoRequest*)req - 1;
    free(pRequest->pData);
    free(pRequest);
}

static void ThreadFuntion(void* arg){
    std::thread t1{[](){
            uv_run(uv_default_loop(), UV_RUN_DEFAULT);
            int i = 0;
            i++;
                   }};
    t1.detach();
    //uv_run(uv_default_loop(), UV_RUN_DEFAULT);


}
