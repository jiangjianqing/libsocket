#include "CmdFactory.h"
#include "protos/udp_msg.discover.pb.h"
#include <stdlib.h>
#include "CryptoUtils.h"
#include <sstream>
#include "protos/tcp_msg.cmd.pb.h"
#include "cmd_def.h"

bool CmdFactory::makeDiscoverMsg(string ip , int port,char*& buf,int& len)
{
    udp_msg::discover msg;
    msg.set_cmd_type("discover");
    msg.set_server_ip(ip);
    msg.set_server_port(port);

    len = msg.ByteSize();

    buf = (char*)malloc(len);

    return msg.SerializeToArray(buf,len);
}

bool CmdFactory::makeIdentifyRequestMsg(char*& buf,int& len)
{
    tcp_msg::IdentifyRequest msg;
    int slen = msg.ByteSize();
    unsigned char* payload = nullptr;
    //make_cmd(cmd_types::MESSAGE,buf,len,payload,slen);
    ///todu: continue 2018.01.21
    return false;
}

string CmdFactory::buf2Str(const char* buf,int len)
{
    ostringstream ostr;
    for(int i = 0; i < len; i++){
        char tmp[3] = {0};
        //添加(unsigned char *)来解决转换时出现ffff问题
        sprintf(tmp, "%02x", ((unsigned char *)buf)[i]);
        ostr<<tmp;
        if(i < len - 1){
            ostr<<" ";
        }
    }
    return ostr.str();
}

/*
    string srcText = "hello";
    string encryptText;
    string encryptHexText;
    string decryptText;

    CryptoUtils::sha256(srcText, encryptText, encryptHexText);


    lm::helloworld msg1;
    msg1.set_id(101);
    msg1.set_str("hello");

      // Write the new address book back to disk.
    fstream output("./log", ios::out | ios::trunc | ios::binary);

    if (!msg1.SerializeToOstream(&output)) {
          cerr << "Failed to write msg." << endl;
          //return -1;
    }
    output.flush();
    output.close();

    lm::helloworld msg2;

    {
        fstream input("./log", ios::in | ios::binary);
        if (!msg2.ParseFromIstream(&input)) {
          cerr << "Failed to parse address book." << endl;
          //return -1;
        }
    }
    */
