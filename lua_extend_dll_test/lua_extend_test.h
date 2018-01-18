#ifndef LUA_EXTEND_TEST_H
#define LUA_EXTEND_TEST_H

#include "lua.hpp"

// my_math.c


#if defined(__linux__)
// Linux系统
//#define LUA_API extern "C"
#elif defined(_WIN32)

//#define LUA_API extern "C" __declspec(dllexport)
// Windows系统
#endif


#ifdef __cplusplus
extern "C" { //2018.01.18 c++ compiler 必须用extern "C" 指定导出
#endif//-----------------------------


//extern "C" int luaopen_mylib(lua_State *L);
LUALIB_API int luaopen_mylib(lua_State *L);



#ifdef __cplusplus//-------------------------------------
}   //end of extern "C" {
#endif

#endif // LUA_EXTEND_TEST_H
