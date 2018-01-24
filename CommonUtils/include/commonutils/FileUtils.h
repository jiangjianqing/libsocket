#ifndef FILEUTILS_H
#define FILEUTILS_H

//#include <xstring>
#include <functional>

#if defined(__linux__)
// Linux系统
#include "boost/filesystem.hpp"
using namespace boost::filesystem;
#elif defined(_WIN32)

// Windows系统
#include <filesystem>
using namespace std::tr2::sys;
namespace fs = std::experimental::filesystem;
#endif

using namespace std;

class FileUtils
{
public:
    FileUtils() = delete;

    //文件扩展名，带点号
    static string extension(const string& filepath);

    //相对路径转为全路径
    static string completePath(const string& relativefilepath);

    //父目录
    static string parentPath(const string& filepath);

    //文件名，不含路径，带扩展名
    static string filename(const string& filepath);

    //不含扩展名的文件名
    static string stem(const string& filepath);

    //文件或目录是否存在
    static bool exists(const string& filepath);

    //当前目录
    static string currentPath();

    //临时目录
    static string tempDirectoryPath();

    //删除文件
    static bool rm(const string& filename);

    //递归删除
    static uintmax_t rm_rf(const string& dirname);

    //复制文件或目录
    static void cp_rf(const string& source,const string& dest);

    static void cp_rf(const string &source, const string &dest,const string& regx);

    //创建目录，支持递归
    static bool mkdirp(const string& dirname);

    //创建目录
    static bool mkdir(const string& dirname);

    static bool isDirectory(const string& dirname);

    static bool isSubDirectory(const string& parentFullpath, const string& childFullpath);

    static int saveStringToFile(const string & filePath, const string & str);

    static void foreachLine(const string& textFilePath, function<void (const string& line)> callback);

    //获得指定目录下的所有文件列表(不包括目录)，支持filter
    static vector<string> ls_R(const string &source,bool useRelativePath,function<bool (const string& filename)> filterCb);
};

#endif // FILEUTILS_H
