#include "LuaEngine.h"
#include <iostream>
#include <string>
#include <sstream>

static LuaEngine* LuaEngine_instance = nullptr;

LuaEngine::LuaEngine():m_luaState(nullptr)
{
    init();
}

LuaEngine::~LuaEngine()
{
    if(m_luaState){
        //4.关闭state,/* 清除Lua */
        lua_close(m_luaState);
    }
}

lua_State* LuaEngine::L()
{
    return m_luaState;
}

void LuaEngine::exec(function<void (lua_State* L)> fn)
{
    lua_State* L = luaL_newstate();//1 创建一个state
    if(m_luaState == nullptr){
        return;
    }
    luaL_openlibs(L);/*2 载入Lua基本库 */

    addLuaPackagePath(L, "path", "./lua/?.lua");
    addLuaPackagePath(L, "cpath", "./lua/?.dll");
    addLuaPackagePath(L, "cpath", "./lua/?.so");

    fn(L);//3 执行callback

    lua_close(L);//4.关闭state,/* 清除Lua */
}

LuaEngine* LuaEngine::getInstance()
{
    if (LuaEngine_instance == NULL) {
        LuaEngine_instance = new LuaEngine();
        LuaEngine_instance->init();
    }
    return LuaEngine_instance;
}

void LuaEngine::destroyInstance(){
    if (LuaEngine_instance) {
        delete LuaEngine_instance;
        LuaEngine_instance = NULL;
    }
}

bool LuaEngine::init(){


    if(m_luaState){
        lua_close(m_luaState);
        m_luaState = nullptr;
    }
    //1.创建一个state
    m_luaState = luaL_newstate();
    if(m_luaState == nullptr){
        return false;
    }
    /* 载入Lua基本库 */
    luaL_openlibs(m_luaState);
    return true;
}

static int average(lua_State *L)
{
    /* 得到参数个数 */
    int n = lua_gettop(L);
    double sum = 0;
    int i;

    /* 循环求参数之和 */
    for (i = 1; i <= n; i++)
    {
        /* 求和 */
        sum += lua_tonumber(L, i);
    }
    /* 压入平均值 */
    lua_pushnumber(L, sum / n);
    /* 压入和 */
    lua_pushnumber(L, sum);
    /* 返回返回值的个数 */
    return 2;
}

int pcall_callback_err_fun(lua_State* L)
{
    lua_Debug debug= {};
    int ret = lua_getstack(L, 2, &debug); // 0是pcall_callback_err_fun自己, 1是error函数, 2是真正出错的函数
    lua_getinfo(L, "Sln", &debug);

    std::string err = lua_tostring(L, -1);
    lua_pop(L, 1);
    std::stringstream msg;
    msg << debug.short_src << ":line " << debug.currentline;
    if (debug.name != 0) {
        msg << "(" << debug.namewhat << " " << debug.name << ")";
    }

    msg << " [" << err << "]";
    err = msg.str();
    lua_pushstring(L, err.c_str());
    return 1;
}

void LuaEngine::addLuaPackagePath(lua_State *ls, const char *name, const char *value)
{
    string v;

    lua_getglobal(ls, "package");
    lua_getfield(ls, -1, name);
    v.append(lua_tostring(ls, -1));
    v.append(";");
    v.append(value);
    lua_pushstring(ls, v.c_str());
    lua_setfield(ls, -3, name);
    lua_pop(ls, 2);
}

void LuaEngine::executeTask(const string& taskname)
{
    auto fn = [&taskname,this](lua_State* L){

        if(L == nullptr){
            return;
        }

        lua_pushcfunction(L, pcall_callback_err_fun);
        int pos_err_fun = lua_gettop(L);

        /* 注册函数 */
        lua_register(L, "average", average);

        //2、加载lua文件
        int iret = luaL_loadfile(L,"lua/task.lua");
        if(iret){
            cout<<"load file error"<<endl;
            return;
        }

        //3、运行lua文件
        iret = lua_pcall(L,0,0,pos_err_fun);
        if(iret){

            int t = lua_type(L, -1);
            const char* err = lua_tostring(L,-1);
            cout<<"pcall error :"<<err<<endl;
            //printf("Error: %s\n", err);
            lua_pop(L, 1);
            return;
        }

        lua_pushstring(L,m_sourceDir.c_str());
        lua_setglobal(L,"source");

        //6、读取函数
        lua_getglobal(L,"execute_task");// 获取函数，压入栈中
        lua_pushstring(L,taskname.c_str());//压入第一个参数
        iret = lua_pcall(L,1/*参数个数*/,1/*结果个数*/,0);// 调用函数，调用完成以后，会将返回值压入栈中，2表示参数个数，1表示返回结果个数。
        if(iret){// 调用出错
            const char* errmsg = lua_tostring(L,-1);
            cout<<errmsg<<endl;
            lua_close(L);
            return;
        }
        if(lua_isnumber(L,-1)){
            double fValue = lua_tonumber(L,-1);
            cout<<"Result is "<<fValue<<endl;
        }
    };

    exec(fn);
}

void LuaEngine::testLua()
{
    auto fn = [](lua_State* L){

        if(L == nullptr){
            return;
        }

        lua_pushcfunction(L, pcall_callback_err_fun);
        int pos_err_fun = lua_gettop(L);

        /* 注册函数 */
        lua_register(L, "average", average);

        /* 运行脚本 */
        luaL_dofile(L, "lua/avg.lua");

        //2、加载lua文件
        int iret = luaL_loadfile(L,"lua/hello.lua");
        if(iret){
            cout<<"load file error"<<endl;
            return;
        }

        //3、运行lua文件
        iret = lua_pcall(L,0,0,pos_err_fun);
        if(iret){

            int t = lua_type(L, -1);
            const char* err = lua_tostring(L,-1);
            cout<<"pcall error :"<<err<<endl;
            //printf("Error: %s\n", err);
            lua_pop(L, 1);
            return;
        }

        //4、读取变量
        lua_getglobal(L,"str");
        string str = lua_tostring(L,-1);
        cout<<"str = "<<str<<endl;

        //5、读取table
        lua_getglobal(L,"tbl");
        lua_getfield(L,-1,"name");
        str = lua_tostring(L,-1);
        cout<<"tbl:name = "<<str<<endl;

        //6、读取函数
        lua_getglobal(L,"add");// 获取函数，压入栈中
        lua_pushnumber(L,10);// 压入第一个参数
        lua_pushnumber(L,20);// 压入第二个参数
        iret = lua_pcall(L,2,1,0);// 调用函数，调用完成以后，会将返回值压入栈中，2表示参数个数，1表示返回结果个数。
        if(iret){// 调用出错
            const char* errmsg = lua_tostring(L,-1);
            cout<<errmsg<<endl;
            lua_close(L);
            return;
        }
        if(lua_isnumber(L,-1)){
            double fValue = lua_tonumber(L,-1);
            cout<<"Result is "<<fValue<<endl;
        }


        //至此，栈中的情况是：
            //=================== 栈顶 ===================
            //  索引  类型      值
            //   4   int：      30
            //   3   string：   shun
            //   2   table:     tbl
            //   1   string:    I am so cool~
            //=================== 栈底 ===================


        /*
        //2.入栈操作
        lua_pushstring(L, "I am so cool~");
        lua_pushnumber(L,20);

        //3.取值操作
        if( lua_isstring(L,1)){             //判断是否可以转为string
            cout<<lua_tostring(L,1)<<endl;  //转为string并返回
        }
        if( lua_isnumber(L,2)){
                cout<<lua_tonumber(L,2)<<endl;
        }*/
    };


    exec((fn));

}
