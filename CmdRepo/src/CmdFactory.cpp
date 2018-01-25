#include "CmdFactory.h"
#include "protos/udp_msg.discover.pb.h"
#include <stdlib.h>
#include "CryptoUtils.h"
#include <sstream>
#include "protos/tcp_msg.package.pb.h"
#include "cmd_def.h"
#include "CmdBufParser.h"
#include "protos/tcp_msg.cmd.file.pb.h"
#include "protos/tcp_msg.cmd.global.pb.h"

#include "commonutils/FileUtils.h"

using namespace google::protobuf;

void packageMsg(const Message& msg,unsigned char*& buf,unsigned& len)
{
    tcp_msg::ProtoPackage pkg;
    pkg.set_type_name(msg.GetTypeName());
    pkg.set_data(msg.SerializeAsString());//比较简洁的写法
    /*//比较复杂的写法
    int tempBufSize = msg.ByteSize();
    unsigned char tempBuf[tempBufSize];
    msg.SerializeToArray(tempBuf,tempBufSize);
    pkg.set_data(tempBuf,tempBufSize);*/

    int slen = pkg.ByteSize();
    unsigned char* payload = (unsigned char*)malloc(slen);
    pkg.SerializeToArray(payload,slen);
    CmdBufParser::makeCmd(cmd_types::MESSAGE,&buf,&len,payload,slen);
    free(payload);
}

//---------------------server request |  client response-------------------

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

bool CmdFactory::makeIdentifyRequestMsg(unsigned char*& buf,unsigned& len)
{
    tcp_msg::global::IdentifyRequest msg;

    packageMsg(msg,buf,len);
    return true;
}

bool CmdFactory::makeIdentifyResponseMsg(unsigned id,unsigned char*& buf,unsigned& len)
{
    tcp_msg::global::IdentifyResponse msg;
    msg.set_id(id);
    packageMsg(msg,buf,len);
    return true;
}

bool CmdFactory::makeStartUpdateRequestMsg(unsigned char*& buf,unsigned& len)
{
    tcp_msg::global::StartUpdateRequest msg;
    packageMsg(msg,buf,len);
    return true;
}

//--------------------以下为client request   server response------------------------

bool CmdFactory::makeFileListRequest(unsigned id,unsigned char*& buf,unsigned& len)
{
    tcp_msg::file::FileListRequest msg;
    msg.set_id(id);
    packageMsg(msg,buf,len);
    return true;
}

bool CmdFactory::makeFileListResponse(const vector<file_brief_info_t*>& files,unsigned char*& buf,unsigned& len)
{
    tcp_msg::file::FileListResponse msg;
    tcp_msg::file::FileInfo* fileinfo;
    for(auto it = files.begin();it != files.end(); it++){
        file_brief_info_t* file = *it;

        fileinfo = msg.add_fileinfo();
        fileinfo->set_filename(file->filename,strlen(file->filename)+1);//连最后一个null一起copy
        fileinfo->set_filesize(file->filesize);
        if(file->checksum_len>0){
            fileinfo->set_checksum(file->checksum,file->checksum_len);
        }
    }
    packageMsg(msg,buf,len);
    return true;
}

bool CmdFactory::makeSendFileRequest(const string& filename,unsigned char*& buf,unsigned& len)
{
    tcp_msg::file::SendFileRequest msg;
    msg.set_filename(filename);
    packageMsg(msg,buf,len);
    return true;
}

bool CmdFactory::makeSendFileResponseMsg(const string& file_name,const char* file_data,const unsigned file_len,unsigned char*& buf,unsigned& len)
{
    tcp_msg::file::SendFileResponse msg;
    msg.set_filename(file_name);
    msg.set_result(tcp_msg::file::SendFileResponse_SendFileResult_WHOLE);
    msg.set_content(file_data,file_len);
    msg.set_type(tcp_msg::file::SendFileResponse_FileType_UNKNOW_FILE_TYPE);
    packageMsg(msg,buf,len);
    return true;
}

file_brief_info_t* CmdFactory::generateFileBriefInfo(const string& filename)
{
    if(FileUtils::exists(filename) == false){
        return nullptr;
    }

    int file_size = FileUtils::getFileSize(filename);

    //注意：此处还没有考虑生成文件校验码
    file_brief_info_t* fileinfo = (file_brief_info_t*)malloc(sizeof(*fileinfo));
    fileinfo->filesize = file_size;
    fileinfo->checksum_len = 0;
    strcpy(fileinfo->filename,filename.data());

    return fileinfo;
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
