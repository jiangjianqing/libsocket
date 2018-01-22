#ifndef CMDDEF_H
#define CMDDEF_H
#include <stdlib.h>
#include "openssl/sha.h"
/*
网络 	Magic 值
main 	F9BEB4D9
testnet 	FABFB5DA
*/
#define NET_MAGIC_MAIN      0xF9BEB4D9
#define NET_MAGIC_TESTNET 	0xFABFB5DA

enum class cmd_types{
    VERSION,VERACK,MESSAGE
};

static const unsigned magic_type_list[] = {NET_MAGIC_MAIN , NET_MAGIC_TESTNET};

typedef struct common_header_s{
    unsigned magic;     //用于识别消息的来源网络，当流状态位置时，它还用于寻找下一条消息
    char command[12];   //识别包内容的ASCII字串，用NULL字符补满，(使用非NULL字符填充会被拒绝)
    unsigned length;    //payload的字节数
}common_header_t;

typedef struct cmd_message_s{
    common_header_t header;
    unsigned checksum;  //sha256(sha256(payload)) 的前4个字节(不包含在version 或 verack 中)
    unsigned char payload[];    //实际数据
}cmd_message_t;

typedef struct cmd_version_s{//command="version";length = n;
    common_header_t header;
    unsigned char payload[];    //实际数据
}cmd_version_t;

typedef struct cmd_verack_s{//command="verack";length = 0;
    common_header_t header;
}cmd_verack_t;

typedef struct xstring_s{//Variable length string  可变长度字符串
    unsigned length;
    char string[];//VLA 变长数组
}xstring_t;


typedef struct netaddr_s{
    unsigned long services;     // 	该连接允许的特性(bitfield)
    //Ipv6地址，以网络字节顺序存储。官方客户端仅支持IPv4，仅读取最后4个字节以获取IPv4地址。
    //IPv4地址以16字节的IPv4映射位址格式写入结构。(12字节 00 00 00 00 00 00 00 00 00 00 FF FF, 后跟4 字节IPv4地址)
    unsigned char ip[16];
    unsigned short port;

}netaddr_t;

#endif // CMDDEF_H
