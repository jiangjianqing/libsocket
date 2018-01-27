#ifndef ABSTRACTCMDPROCESSOR_H
#define ABSTRACTCMDPROCESSOR_H

#include <string>
#include <vector>
#include <functional>
#include "CmdBufParser.h"
#include "CmdFactory.h"

using namespace std;

//CMD_EVENT_TYPE_USERID指定了用户可以自定义消息ID的起始序号
#define CMD_EVENT_TYPE_ID_HIGEST 1024
//#define ID_CMDPROCESSER_THREADEVENT   wxID_HIGHEST+30
enum class CmdEventType{
    UNKNOW,UdpDiscover,TcpIdentifyResponse,
    TcpFileListRequest,TcpSendFileRequest_NextFile,TcpSendFileRequest_CurrentFile,
    TcpExecuteTaskRequest
};

//eventTypeId指向在CmdEventTYpe中定义的ID、或用户自定义的ID
typedef function<void (const int& eventTypeId)> CmdEventCb;

class AbstractClientCmdProcessor : public CmdBufParser
{
public:
    explicit AbstractClientCmdProcessor();
    ~AbstractClientCmdProcessor();

    void recvUdpData(const char* buf,int nread);

    string serverIp(){return m_serverIp;}
    int serverPort(){return m_serverPort;}
    //const vector<string>& filenameList(){return m_filenameList;}
    string getCurrFilename();
    bool toNextFilename();
    //取得当前文件的StartPos，服务器会从该位置开始读取
    int64_t getCurrFileStartPos();

    void setIdentifyResponseId(int value){m_identifyResponseId = value;}

    int identifyResponseId(){return m_identifyResponseId;}

    string currTaskname(){return m_currTaskname;}

protected:
    //特别注意，该路径一定要是某个子目录
    string m_recvFilePath;
    virtual void callCmdEventCb(const int& eventTypeId) = 0;
    //无法解析buf为ProtoPackage时会调用，由sub class处理
    virtual void onRecvUnknowMessage(const unsigned char* buf,const unsigned len) =0;
    //无法根据type_name创建protobuf时调用
    virtual void onRecvUnknowProtoPackage(const unsigned char* buf,const unsigned len) = 0;
    virtual void onRecvErrorProtoPackage(const unsigned char* buf,const unsigned len) = 0;
private:
    string m_serverIp;
    int m_serverPort;


    int m_identifyResponseId;
    vector<file_brief_info_t*> m_fileBriefInfoList;
    int m_CurrFileNameListIndex;//当前待获取文件名的index

    string m_currTaskname;

    void clearFileBriefInfoList();
    void pushFileBriefInfoToList(file_brief_info_t* fileBriefInfo);

    void onRecvCmd(const unsigned char* buf,const unsigned len) override;
};

#endif // ABSTRACTCMDPROCESSOR_H
