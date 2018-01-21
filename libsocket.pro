TEMPLATE = subdirs

SUBDIRS += \
#linux 下 qt+protobuf 会报错，qtServer目前只用于windows下
    #qtServer  \
    CryptoUtils \
    lua_extend_dll_test \
    Socket  \
    CmdRepo \
    wxServer    \
    wxClient

wxClient.depends = CmdRepo Socket
wxServer.depends = CmdRepo Socket
