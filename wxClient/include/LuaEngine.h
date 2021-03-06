#ifndef LUAENGINE_H
#define LUAENGINE_H

#include "lua.hpp" //注：Lua header files for C++
#include <functional>

using namespace std;

class LuaEngine
{
public:
    explicit LuaEngine();
    virtual ~LuaEngine();

    void testLua();
    void executeTask(const string& taskname);

    static LuaEngine* getInstance();
    static void destroyInstance();

    void setSourceDir(const wstring& dirname){m_sourceDir = dirname;}
    void setDestDir(const wstring& dirname){m_destDir = dirname;}

    int identifyResponseId(){return m_identifyResponseId;}
    void loadParamFromLuaScript();

private:
    lua_State* m_luaState;

    wstring m_sourceDir;
    wstring m_destDir;

    int m_identifyResponseId;

    bool init();

    lua_State* L();

    void exec(function<void (lua_State* L)> fn);
    void addLuaPackagePath(lua_State *ls, const char *name, const char *value);

};

#endif // LUAENGINE_H
