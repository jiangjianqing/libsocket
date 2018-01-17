#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <lua.hpp>  
  
static int c_cont (lua_State *L, int status, lua_KContext ctx)  
{  
    printf("c_count status:%d\n",status);  
    printf("c_count ctx:%d\n", ctx);  
    return 0;  
}  
  
static int c_callback(lua_State *L){  
  /* 使用 lua_pcallk，而不是lua_pcall */  
    lua_getglobal(L, "run");  
    int ret = lua_pcallk(L, 0, 0, 0, 0, c_cont);  
    printf("ret = %d\n",ret);  
    if(ret) {  
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));  
        lua_pop(L, 1);  
        exit(1);  
    }  
    return 0;  
}  
  
int main()  
{  
    struct lua_State * L;  
    L = luaL_newstate();  
    luaL_openlibs(L);  
    int ret = luaL_dofile(L,"main.lua");  
    if (ret != LUA_OK)  
    {  
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));  
        lua_pop(L, 1);  
        return 0;  
    }  
    while (true)  
    {  
        printf("c:runing lua\n");  
//      lua_getglobal(L, "run");  
//      lua_pcall(L, 0, 0, 0);  
        c_callback(L);  
        sleep(1);  
    }  
    return 0;  
} 
