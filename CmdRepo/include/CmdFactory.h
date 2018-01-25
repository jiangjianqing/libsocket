#ifndef CMDFACTORY_H
#define CMDFACTORY_H

#include <string>
#include <vector>

using namespace std;

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

typedef struct file_brief_info_s{
    char filename[MAX_PATH];//MAX_PATH
    uint64_t filesize;
    int checksum_len;
    unsigned char checksum[];
}file_brief_info_t;

class CmdFactory
{
public:
    CmdFactory() = delete;

    //---------------------server request |  client response-------------------

    static bool makeDiscoverMsg(string ip , int port,char*& buf,int& len);

    static bool makeIdentifyRequestMsg(unsigned char*& buf,unsigned& len);
    static bool makeIdentifyResponseMsg(unsigned id,unsigned char*& buf,unsigned& len);

    static bool makeStartUpdateRequestMsg(unsigned char*& buf,unsigned& len);

    //--------------------以下为client request   server response------------------------

    static bool makeFileListRequest(unsigned id,unsigned char*& buf,unsigned& len);
    static bool makeFileListResponse(const vector<file_brief_info_t*>& fileinfos,unsigned char*& buf,unsigned& len);

    static bool makeSendFileRequest(const string& filename,uint64_t startpos,unsigned char*& buf,unsigned& len);
    static bool makeSendFileResponseMsg(const string& filename,const char* file_data,const unsigned file_len,unsigned char*& buf,unsigned& len);

    //生成文件的摘要信息
    static file_brief_info_t* generateFileBriefInfo(const string& filename);
    //从protobuf 的 FileInfo结构生成file_brief_info, 输入参数为 tcp_msg::file::FileInfo 的指针
    static file_brief_info_t* generateFileBriefInfoFromProtobufFileInfo(const void* data);

    static string buf2Str(const char* buf,int len);


};

#endif // CMDFACTORY_H
