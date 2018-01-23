#ifndef CMDFACTORY_H
#define CMDFACTORY_H

#include <string>

using namespace std;

class CmdFactory
{
public:
    CmdFactory() = delete;

    static bool makeDiscoverMsg(string ip , int port,char*& buf,int& len);

    static bool makeIdentifyRequestMsg(unsigned char*& buf,unsigned& len);
    static bool makeIdentifyResponseMsg(unsigned id,unsigned char*& buf,unsigned& len);

    static string buf2Str(const char* buf,int len);


};

#endif // CMDFACTORY_H
