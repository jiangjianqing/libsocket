--搜索指定路径下，以 .lua结尾的文件
--package.path = "../h/?.lua;../ybslib/src/?.lua;"..package.path
--搜索指定路径下，以.so 或.dll 结尾的文件
--package.cpath = "../ybslib/bin/?.so;"..package.cpath
--package.cpath = "../ybslib/bin/?.dll;"..package.cpath

local mylib= require "mylib"
mylib.sayHello()
avg,sum=mylib.average(10,20,30)
print("avg = "..avg..",sum="..sum)   
str = "I am so cool"  
tbl = {name = "shun", id = 20114442}  
function add(a,b)  
    print("---print-in---lua----------")    
    return a + b  
    
end


