#include "FileUtils.h"
#include <fstream>
#include <iostream>

//C++11 提供的filesystem提供了丰富的文件操作的类和函数,可以跨平台
#include <regex>

//namespace fs = std::experimental::filesystem;



/*范例：定义一个path对象
std::tr2::sys::path p("d:/test/test.txt");
std::tr2::sys::path p2("mytest.cpp");
std::tr2::sys::path p3;
*/

bool FileUtils::exists(const std::string& filename)
{
    return exists(filename);

    //fstream file;
    //file.open(filename,ios::out);
    //return !file ? false : true;
}

std::string FileUtils::currentPath()
{
    return current_path().string();
}

std::string FileUtils::tempDirectoryPath()
{
    return temp_directory_path().string();
}

bool FileUtils::rm(const std::string& filename)
{
    return remove(filename);
}

uintmax_t FileUtils::rm_rf(const std::string& dirname)
{
    return remove_all(dirname);
}

void FileUtils::cp_rf(const std::string& source,const std::string& dest)
{
    copy(source, dest);
}

void FileUtils::cp_rf(const std::string &source, const std::string &dest,const std::string& regx)
{
    std::regex re(regx);
#if defined(__linux__)
    for(auto& fe: directory_iterator(source.c_str())){
#elif defined(_WIN32)
    //复制文件
    for(auto& fe: fs::directory_iterator(source.c_str())){
#endif
        path fp = fe.path();
        std::string filename = fp.filename().string() ;
        std::string fullpathname = fp.string();

        //2017.12.11 这里还需要处理子目录
        if(std::regex_match(filename,re)){
            std::string destFileName = dest+"/"+filename;
#if defined(__linux__)
            copy_file(fullpathname,destFileName,copy_option::overwrite_if_exists);
#elif defined(_WIN32)
            copy_file(fullpathname,destFileName,copy_options::overwrite_existing);
#endif
        }
    }
}

bool FileUtils::mkdirp(const std::string& dirname)
{
    return create_directories(dirname);
}

bool FileUtils::mkdir(const std::string& dirname)
{
    return create_directory(dirname);
}

bool FileUtils::isDirectory(const std::string& dirname)
{
#if defined(__linux__)
// Linux系统
    return is_directory(dirname);
#elif defined(_WIN32)

// Windows系统
    return fs::is_directory(dirname);
#endif

}

std::string FileUtils::extension(const std::string& filename)
{
    path p(filename);
    return p.extension().string();
}

std::string FileUtils::filename(const std::string& filepath)
{
    path p(filepath);
    return p.filename().string();
}

std::string FileUtils::stem(const std::string& filepath)
{
    path p(filepath);
    return p.stem().string();
}

std::string FileUtils::completePath(const std::string& relativefilepath)
{
    return system_complete(relativefilepath).string();
}

std::string FileUtils::parentPath(const std::string& filepath)
{
    path p(filepath);
    return p.parent_path().string();
}

int FileUtils::saveStringToFile(const std::string & filePath, const std::string& str)
{
    std::ofstream   OsWrite(filePath,std::ofstream::out);
    OsWrite<<str;
    OsWrite<<std::endl;
    OsWrite.close();
   return 0;
}

void FileUtils::foreachLine(const std::string& textFilePath, std::function<void (const std::string& line)> callback)
{
    // 最好对file1和file2进行判断

    std::ifstream in(textFilePath);
    //ofstream out(file2);
    std::string line;

    while (getline (in, line))
    {
        callback(line);
            //out << line << endl;
    }
}

bool FileUtils::isSubDirectory(const std::string& parentFullpath, const std::string& childFullpath)
{
    path parentPath(parentFullpath);
    path childPath(childFullpath);
    //path temp = algPath;
    bool ret = false;
    ret = childPath.string().find(parentPath.string()) != std::string::npos;
    /*
     * //遍历模式效率不行
    while(temp.has_parent_path()){

        string p1 = temp.string();
        if(strAlgRepoPath == p1){
            ret = true;
            break;
        }

        temp = temp.parent_path();
    }*/

    return ret;
}

string  replace_all_str(const string&   source,const   string&   old_value,const   string&   new_value){
    string newStr = source;
    while(true)   {
        string::size_type   pos(0);
        if(   (pos=newStr.find(old_value))!=string::npos   )
            newStr.replace(pos,old_value.length(),new_value);
        else
            break;
    }
    return   std::move(newStr);
}

vector<string> FileUtils::ls_R(const string &dirname,bool saveRelativePath,function<bool (const string& filename)> filterCb)
{
    vector<string> list;

#if defined(__linux__)
    for(auto& fe: directory_iterator(dirname.c_str())){
#elif defined(_WIN32)
    //复制文件
    for(auto& fe: fs::directory_iterator(dirname.c_str())){
#endif
        path fp = fe.path();
        std::string filename = fp.filename().string() ;
        std::string fullpathname = fp.string();

        if(is_directory(fullpathname)){
            vector<string> subList = ls_R(fullpathname,saveRelativePath,filterCb);
            //todo: 附加
        }else{
            if(!filterCb || filterCb(fullpathname)){
                if(saveRelativePath){
                    list.push_back(replace_all_str(fullpathname,dirname,""));
                }else{
                    list.push_back(fullpathname);
                }
            }
        }
    }

    return std::move(list);
}
