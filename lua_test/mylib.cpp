    #include <stdio.h>  
    #include <stdlib.h>  
    #include <string.h>  
    #include <unistd.h>  
    #include <lua.hpp>  
      
    extern "C" int add(struct lua_State * L)  
    {  
        double op1 = luaL_checknumber(L,1);  
        double op2 = luaL_checknumber(L,2);  
        lua_pushnumber(L,op1 + op2);  
        return 1;  
    }  
      
    static struct luaL_Reg libs[] = {  
            {"add", add},  
            {NULL, NULL}  
    };  
      
    extern "C" int luaopen_mylib(struct lua_State * L)  
    {  
        luaL_newlib(L, libs);  
        return 1;  
    }  
