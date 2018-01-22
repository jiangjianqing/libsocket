#ifndef CMDBUFPARSER_H
#define CMDBUFPARSER_H

#include "cmd_def.h"
#include <functional>
#define MAX_CMD_PARSER_BUF_LENGTH 1024*1024

using namespace std;

typedef function<void(const unsigned char* buf,const unsigned len)> CmdBufParserCb;

class CmdBufParser
{
public:
    CmdBufParser();
    virtual ~CmdBufParser();

    void inputData(const unsigned char* buf , const unsigned len);
    void cleanParserBuf(){resetParserBuf(nullptr,0,-1);}
    static bool isSingleEntireCmd(const unsigned char* buf , const unsigned len);//是否整个buf为一个完整指令

    void setCmdBufParserCb(CmdBufParserCb cb){m_parserCb = cb;}

    static unsigned  getChecksum(const unsigned char* payload,const unsigned len);
    static int makeCmd(cmd_types type,unsigned char** dest,unsigned* dlen,const unsigned char* payload,const unsigned slen);
private:
    CmdBufParserCb m_parserCb;
    unsigned char* m_parserBuf;
    unsigned m_bufTailPos;//parserBuf的最后位置，用于填充数据
    //obsoleted，设计简化,指令头永远在0位置
    int m_bufHeaderPos;//指示命令头的位置（取值只有 0和-1），没有找到命令头则 = -1，否则 = 0

    void resetParserBuf(const unsigned char* buf,const unsigned len,int newHeaderPos);
    void appendToParserBuf(const unsigned char* buf,const unsigned len);

    void processParserBuf();//处理parserBuf，获取可用指令
    static int findCmdHeader(const unsigned char* buf,const unsigned len);
    static int checkCmd(const unsigned char* buf,const unsigned len,const int headerPos);
};

#endif // CMDBUFPARSER_H
