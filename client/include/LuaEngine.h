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

    static LuaEngine* getInstance();
    static void destroyInstance();

private:
    lua_State* m_luaState;

    bool init();

    lua_State* L();

    void exec(function<void (lua_State* L)> fn);
    void addLuaPackagePath(lua_State *ls, char *name, char *value);

};

#endif // LUAENGINE_H
