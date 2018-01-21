TEMPLATE = subdirs

SUBDIRS += \
    #qtServer  \
    CryptoUtils \
    lua_extend_dll_test \
    Socket  \
    CmdRepo \
    #wxServer    \
    wxClient

wxClient.depends = CmdRepo Socket
wxServer.depends = CmdRepo Socket
