#include <stdio.h>
#include "lua_extend_test.h"
#include <math.h>

static int my_math_sin (lua_State *L) {
    lua_pushnumber(L, sin(luaL_checknumber(L, 1)));
    return 1;
}

static int my_math_cos (lua_State *L) {
    lua_pushnumber(L, cos(luaL_checknumber(L, 1)));
    return 1;
}

static int averageFunc(lua_State *L)
{
    int n = lua_gettop(L);
    double sum = 0;
    int i;

    /* 循环求参数之和 */
    for (i = 1; i <= n; i++)
        sum += lua_tonumber(L, i);

    lua_pushnumber(L, sum / n);     //压入平均值
    lua_pushnumber(L, sum);         //压入和

    return 2;                       //返回两个结果
}

static int sayHelloFunc(lua_State* L)
{
    printf("hello world!");
    return 0;
}

static const struct luaL_Reg myLib[] =
{
    {"average", averageFunc},
    {"sayHello", sayHelloFunc},
    {"my_cos",   my_math_cos},
    {"my_sin",   my_math_sin},
    {NULL, NULL}       //数组中最后一对必须是{NULL, NULL}，用来表示结束
};

/*
** Open my_math library
*/
int luaopen_mylib(lua_State *L)   {
//#ifdef lua5.1
//5.1 下直接使用luaL_register 就好
//    luaL_register(L, "libname", funcs);
//#else //lua5.2
    lua_newtable(L);
    //先把一个table压入VS，然后在调用luaL_setfuncs就会把所以的func存到table中
    //注意不像luaL_register这个table是个无名table，可以在的使用只用一个变量来存入这个table。
    //e.g local clib = require "libname". 这样就不会污染全局环境。比luaL_register更好。
    luaL_setfuncs(L, myLib, 0);
//#endif
  return 1;
}
