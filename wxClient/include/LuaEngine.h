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

    void setSourceDir(const string& dirname){m_sourceDir = dirname;};
    void setDestDir(const string& dirname){m_destDir = dirname;}

private:
    lua_State* m_luaState;

    string m_sourceDir;
    string m_destDir;

    bool init();

    lua_State* L();

    void exec(function<void (lua_State* L)> fn);
    void addLuaPackagePath(lua_State *ls, const char *name, const char *value);

};

#endif // LUAENGINE_H
