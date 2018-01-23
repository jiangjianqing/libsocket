#include "net_base.h"
TcpClientCtx* AllocTcpClientCtx(void* parentserver)
{
    TcpClientCtx* ctx = (TcpClientCtx*)malloc(sizeof(*ctx));
    ctx->packet_ = new PacketSync;
    ctx->read_buf_.base = (char*)malloc(BUFFER_SIZE);
    ctx->read_buf_.len = BUFFER_SIZE;
    ctx->write_req.data = ctx;//store self
    ctx->parent_server = parentserver;//store TCPClient
    ctx->parent_acceptclient = NULL;
    return ctx;
}

void FreeTcpClientCtx(TcpClientCtx* ctx)
{
    delete ctx->packet_;
    free(ctx->read_buf_.base);
    free(ctx);
}


write_param* AllocWriteParam(void)
{
    write_param* param = (write_param*)malloc(sizeof(*param));
    param->buf_.base = (char*)malloc(BUFFER_SIZE);
    param->buf_.len = BUFFER_SIZE;
    param->buf_truelen_ = BUFFER_SIZE;
    return param;
}

void FreeWriteParam(write_param* param)
{
    free(param->buf_.base);
    free(param);
}

udp_send_param * AllocUdpSendParam(void)
{
    udp_send_param* param = (udp_send_param*)malloc(sizeof(*param));
    param->buf_.base = (char*)malloc(BUFFER_SIZE);
    param->buf_.len = BUFFER_SIZE;
    param->buf_truelen_ = BUFFER_SIZE;
    return param;
}
void FreeUdpSendParam(udp_send_param* param)
{
    free(param->buf_.base);
    free(param);
}


UdpClientCtx* AllocUdpClientCtx(void* parentserver)
{
    UdpClientCtx* ctx = (UdpClientCtx*)malloc(sizeof(*ctx));
    ctx->packet_ = new PacketSync;
    ctx->read_buf_.base = (char*)malloc(BUFFER_SIZE);
    ctx->read_buf_.len = BUFFER_SIZE;
    ctx->write_req.data = ctx;//store self
    ctx->parent_server = parentserver;//store UdpClient
    //ctx->parent_acceptclient = NULL;
    return ctx;
}
void FreeUdpClientCtx(UdpClientCtx* ctx)
{
    delete ctx->packet_;
    free(ctx->read_buf_.base);
    free(ctx);
}
