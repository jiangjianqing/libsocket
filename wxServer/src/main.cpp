// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "MainFrame.h"
#include "TcpServer.h"

#include <iostream>

using namespace std;
/*
int main()
{
    cout << "Hello World!" << endl;
    return 0;
}*/

class MyApp: public wxApp
{
    //IMPLEMENT_APP中会调用wxApp的OnInit方法，窗体的初始化工作应该放在这里进行
    //返回true代表正常，后续的OnRun和OnExit会被调用；返回false程序将退出
    bool OnInit()
    {
        //Frame和Dialog的区别:
        //一般情况下，有菜单工具栏的是Frame，没有的是Dialog。
        //当然这是一般情况，因为你完全可以把Dialog做成Frame样子。
        //Frame一般用做主窗体，可以有菜单、工具栏、状态栏。Dialog用做模式窗口，可以返回不同的值，比如打开个设置对话框、确认对话框等等

        /*
        wxFrame::wxFrame( wxWindow *  parent,
                wxWindowID      id,
                const wxString &    title,
                const wxPoint &     pos = wxDefaultPosition,
                const wxSize &      size = wxDefaultSize,
                long    style = wxDEFAULT_FRAME_STYLE,
                const wxString &    name = wxFrameNameStr
            )
               */

        //parent是NULL, 没有父窗体（因为它是顶级窗体），wxID_ANY（其实就是-1）表示让wx安排一个ID（wx中的各控件用ID来识别）
        MainFrame *frame = new MainFrame();
        //wxFrame* frame = new wxFrame(NULL, wxID_ANY, "Our First App");
        frame->Show();
        return true;
          //init
        }
};

//。wx中入口函数当然也是main，只是main函数已经在wx库中被定义好了，不需要再重新定义了。可以从源码wx/app.h中找到main函数被定义在宏wxIMPLEMENT_WXWIN_MAIN中：
//IMPLEMENT_APP(app)宏调用了wxIMPLEMENT_WXWIN_MAIN并且完成了许多wx初始化的工作，所以在我们的程序中，只需加上IMPLEMENT_APP(OurApp)作为入口即可，预处理器会自动替换…
//上面的IMPLEMENT_APP宏接受一个wxApp在类型，即OurApp必需是wxApp类型或wxApp的派生类型，所以我们先定义一个继承自wxApp的类OurApp：
wxIMPLEMENT_APP(MyApp);


