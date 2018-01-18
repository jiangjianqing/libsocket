#ifndef LUA_EXTEND_TEST_H
#define LUA_EXTEND_TEST_H

#include "lua.hpp"

// my_math.c



/*
** Open my_math library
*/
//LUALIB_API int luaopen_mylib(lua_State *L);

//2018.01.17必须用extern "C" 指定导出
//extern "C" int luaopen_mylib(lua_State *L);
extern "C" __declspec(dllexport) int luaopen_mylib(lua_State *L);

#endif // LUA_EXTEND_TEST_H
