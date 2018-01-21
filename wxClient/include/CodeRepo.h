#ifndef CODEREPO_H
#define CODEREPO_H

#include "wx/event.h"

class CodeRepo
{
public:
    CodeRepo();

    /**
     * @brief 动态库调用和文件名操作范例
     */
    void DynamicLibraryExample();

    /**
     * @brief OnChar 字符过滤事件处理范例
     * @param event
     */
    void OnChar(wxKeyEvent& event);
};

#endif // CODEREPO_H
