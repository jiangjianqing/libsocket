--搜索指定路径下，以 .lua结尾的文件
--package.path = "../h/?.lua;../ybslib/src/?.lua;"..package.path
--搜索指定路径下，以.so 或.dll 结尾的文件
--package.cpath = "../ybslib/bin/?.so;"..package.cpath
--package.cpath = "../ybslib/bin/?.dll;"..package.cpath

local fs= require "xfilesystem"

--全局变量，由外部设置
source = "/home/cz_jjq/Desktop"
local dest = "/home/cz_jjq/123456"

function upgrade()  
    return fs.cp_rf(source,dest)
    --return fs.cp_rf(,)
end

function execute_task(taskname)
    fs.cp_rf(source,dest)
    return 9
end;

--mylib.cp_rf("/home/cz_jjq/Desktop","/home/cz_jjq/123456")
--mylib.rm_rf("/home/cz_jjq/123456")
--local bret = mylib.mkdirp("/home/cz_jjq/123/456/789")
--local bret = execute_task("upgrade")
--print(bret)


