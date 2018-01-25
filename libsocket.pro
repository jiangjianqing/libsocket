TEMPLATE = subdirs

SUBDIRS += \
#linux 下 qt+protobuf 会报错，qtServer目前只用于windows下
    #qtServer  \
    CryptoUtils \
    lua_extend_dll_test \
    Socket  \
    CmdRepo \
    CommonUtils \
    wxServer    \
    wxClient    \
    UvSocket

wxClient.depends = CmdRepo Socket CryptoUtils
wxServer.depends = CmdRepo Socket CryptoUtils
