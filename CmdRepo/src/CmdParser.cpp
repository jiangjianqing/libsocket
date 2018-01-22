#include "CmdParser.h"
#include <stdlib.h> //calloc、malloc
#include <string.h> //memset
#include <stdexcept>
#include <assert.h>
#include "CmdParser.h"

using namespace std;

CmdParser::CmdParser()
{
    m_parserBuf = static_cast<unsigned char*>(calloc(1,MAX_CMD_PARSER_BUF_LENGTH));
    //m_parserBuf = malloc(MAX_CMD_PARSER_BUF_LENGTH);
    //memset(m_parserBuf,0,MAX_CMD_PARSER_BUF_LENGTH);
    cleanParserBuf();
}

CmdParser::~CmdParser()
{
    free(m_parserBuf);
}

void CmdParser::resetParserBuf(const unsigned char* buf,const unsigned len,int newHeaderPosInBuf)
{
    memset(m_parserBuf,0,MAX_CMD_PARSER_BUF_LENGTH);
    if(buf == nullptr){
        m_bufTailPos = 0;
        m_bufHeaderPos = -1;
    }else{
        int newLen = len - newHeaderPosInBuf;
        memcpy(m_parserBuf,buf+newHeaderPosInBuf,newLen);
        m_bufHeaderPos = 0;//指令头永远在0位置
        m_bufTailPos = newLen;
    }
}

void CmdParser::appendToParserBuf(const unsigned char* buf,const unsigned len)
{
    memcpy(m_parserBuf+m_bufTailPos,buf,len);
    m_bufTailPos += len;
}

unsigned  CmdParser::getChecksum(const unsigned char* payload,const unsigned len)
{
    unsigned iret = 0;
    // 调用sha256哈希
    unsigned char md[33] = {0};
    SHA256(payload,len,md);
    memcpy(&iret,md,sizeof(unsigned));
    return iret;
}

int CmdParser::makeCmd(cmd_types type,unsigned char** dest,unsigned* dlen,const unsigned char* payload,const unsigned slen)
{
    common_header_t* header = NULL;

    switch(type){
    case cmd_types::VERSION:
        break;
    case cmd_types::VERACK:
        break;
    case cmd_types::MESSAGE:
        *dlen = sizeof(cmd_message_t) + slen;
        header  = (common_header_t*)calloc(1 , *dlen);
        cmd_message_t* msg = (cmd_message_t*)header;
        memcpy(header->command,"message",7);
        memcpy(msg->payload,payload,slen);
        msg->checksum = getChecksum(payload,slen);
        break;
    }
    header->magic = NET_MAGIC_MAIN;
    header->length = slen;
    *dest = (unsigned char*)header;

    return 0;
}

void CmdParser::inputData(const unsigned char* buf , const unsigned len)
{
    assert(len < MAX_CMD_PARSER_BUF_LENGTH - 1024);
    /*if(len >= MAX_CMD_PARSER_BUF_LENGTH){
        throw runtime_error("len > MAX_CMD_PARSER_BUF_LENGTH");
    }*/

    int newHeaderPosInTempBuf = findCmdHeader(buf,len);
    if(newHeaderPosInTempBuf == -1 && m_bufHeaderPos == -1){//没有找到指令头，则数据都忽略
        resetParserBuf(nullptr,0,-1);
        return;
    }
    if(newHeaderPosInTempBuf == 0 || m_bufHeaderPos == -1){//如果开头就是header或parserBuf中不存在指令头,则清除所有parserBuf
        resetParserBuf(buf,len,newHeaderPosInTempBuf);
    }else{
        int remainedDataLen = len - newHeaderPosInTempBuf;//上次遗留数据长度
        if(m_bufTailPos + remainedDataLen >= MAX_CMD_PARSER_BUF_LENGTH){ //数据总长超过了缓冲区长度,把最后命令
            resetParserBuf(buf,len,newHeaderPosInTempBuf);
        }else{
            appendToParserBuf(buf,remainedDataLen);//处理上次遗留数据
            processParserBuf();
            resetParserBuf(buf,len,newHeaderPosInTempBuf);//处理后续指令
        }
    }

    processParserBuf();//从parserBuf中获取指令
}

int CmdParser::checkCmd(const unsigned char* buf,const unsigned len,const int headerPos)
{
    int availdBufLen = len - headerPos;
    if(availdBufLen < sizeof(cmd_message_t)){//2018.01.22 当前只支持识别cmd_message_t
        return -1;//最小长度不足
    }
    cmd_message_t* cmdMsg = (cmd_message_t*)(buf+headerPos);

    if(cmdMsg->header.length < availdBufLen - sizeof(cmd_message_t)){
        return -2;//payload长度不足
    }
    if(cmdMsg->header.length > 0){
        if(cmdMsg->checksum != getChecksum(cmdMsg->payload,cmdMsg->header.length)){
            return -3;//校验码不正确
        }
    }
    return 0;
}

void CmdParser::processParserBuf()
{
    int remainedDataLen = 0;// = 0时不需要进行递归处理
    int cmdTotalLen = 0;
    assert(m_bufHeaderPos != -1);
    int iret = checkCmd(m_parserBuf,m_bufTailPos,0/*m_bufHeaderPos*/);
    if(iret == 0){
        cmd_message_t* cmdMsg = (cmd_message_t*)(m_parserBuf);//2018.01.22 当前只支持识别cmd_message_t
        cmdTotalLen = sizeof(cmd_message_t) + cmdMsg->header.length;
        //取到命令，通知回调
        unsigned char* cmdBufTemp = (unsigned char*)malloc(cmdTotalLen);
        memcpy(cmdBufTemp,m_parserBuf,cmdTotalLen);
        if(m_cmdParserCb){//可以考虑加入多线程，提升处理能力
            m_cmdParserCb(cmdBufTemp,cmdTotalLen);
        }
        free(cmdBufTemp);
        //----继续处理下一条命令-----
        if(cmdTotalLen == m_bufTailPos){
            cleanParserBuf();
        }else{
            remainedDataLen = m_bufTailPos - cmdTotalLen;
        }
        return;
    }else if(iret == -3){//校验码不正确
        //可以考虑输出该部分数据

        //----继续处理下一条命令-----
        cmd_message_t* cmdMsg = (cmd_message_t*)(m_parserBuf);//2018.01.22 当前只支持识别cmd_message_t
        cmdTotalLen = sizeof(cmd_message_t) + cmdMsg->header.length;
        remainedDataLen = m_bufTailPos - cmdTotalLen;
    }
    if(remainedDataLen > 0 ){//有一条cmd已经被处理，需要recursive处理下一条指令
        int newHeaderPos = findCmdHeader(m_parserBuf + cmdTotalLen,remainedDataLen);
        if(newHeaderPos == -1){
            cleanParserBuf();
        }else{
            memmove(m_parserBuf,m_parserBuf + cmdTotalLen , remainedDataLen);
            m_bufTailPos = remainedDataLen;
            m_bufHeaderPos = 0;

            processParserBuf();//再次递归处理
        }
    }
}

bool CmdParser::isSingleEntireCmd(const unsigned char* buf , const unsigned len)
{
    int headerPos = findCmdHeader(buf,len);
    if(headerPos != 0){
        return false;
    }

    return checkCmd(buf,len,headerPos) == 0;
}

int CmdParser::findCmdHeader(const unsigned char* buf,const unsigned len)
{
    int headerPos = -1;
    bool found = false;
    int count = sizeof(magic_type_list) / sizeof(unsigned);//获得magic数量
    for(int k = 0; k < count;k++){
        unsigned magic_type = magic_type_list[k];
        for(int i = 0; i < len; i++){
            if(memcmp(buf+i,(const void *)&magic_type,sizeof(unsigned))==0){
                headerPos = i;
                found = true;
                break;
            }
        }
        if(found){
            break;
        }
    }

    return headerPos;
}
