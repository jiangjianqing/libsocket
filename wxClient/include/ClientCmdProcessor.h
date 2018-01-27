#ifndef CLIENTCMDPROCESSOR_H
#define CLIENTCMDPROCESSOR_H

#include <string>
#include <vector>
#include <functional>
#include "wx/event.h"
#include "LuaEngine.h"
#include "CmdBufParser.h"
#include "CmdFactory.h"
#include "AbstractClientCmdProcessor.h"

using namespace std;

class ClientCmdProcessor : public AbstractClientCmdProcessor
{
public:
    explicit ClientCmdProcessor(wxEvtHandler* evtHandler);
    ~ClientCmdProcessor();



protected:
    virtual void callCmdEventCb(const int& eventTypeId);
    //无法解析buf为ProtoPackage时会调用，由sub class处理
    virtual void onRecvUnknowMessage(const unsigned char* buf,const unsigned len);
    //无法根据type_name创建protobuf时调用
    virtual void onRecvUnknowProtoPackage(const unsigned char* buf,const unsigned len);
    virtual void onRecvErrorProtoPackage(const unsigned char* buf,const unsigned len);
private:
    wxEvtHandler* m_wxEvtHandler;

    LuaEngine m_luaEngine;

};

#endif // CLIENTCMDPROCESSOR_H
