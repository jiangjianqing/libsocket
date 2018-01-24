#ifndef CMDFACTORY_H
#define CMDFACTORY_H

#include <string>
#include <vector>

using namespace std;

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
    static bool makeFileListResponse(const vector<string>& filenames,unsigned char*& buf,unsigned& len);

    static bool makeSendFileRequest(const string& filename,unsigned char*& buf,unsigned& len);
    static bool makeSendFileResponseMsg(const string& file_name,const char* file_data,const unsigned file_len,unsigned char*& buf,unsigned& len);

    static string buf2Str(const char* buf,int len);


};

#endif // CMDFACTORY_H
