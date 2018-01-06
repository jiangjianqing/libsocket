#ifndef MYTRAY_H
#define MYTRAY_H

#include "wx/taskbar.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "MyFrame.h"

class MyFrame;

/*
使用 wxTaskBarIcon 的退出流程
  1. 在 wxTaskBarIcon 上点'退出'，调用 m_pMainFrame->Close()
  2. MainFrame 收到消息，最后调用自己的 wxFrame::Destroy()
  3. 进入析构函数，delete m_pTray，通过 wxTaskBarIcon::~wxTaskBarIcon() 完成退出流程

因为在 win32 下，wxTaskBarIcon 后面隐含着一个 TopLevelWindow，而此 TopLevelWindow 不退出，app 也是不会退出的。
*/

class MyTray : public wxTaskBarIcon
{
public:
    MyTray(MyFrame* mainFrame);

    void OnAbout(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    /*
    template <typename EventTag, typename Class, typename EventArg>
    void bindClose(const EventTag &eventType,void (Class::*method)(EventArg &));
    */

protected:
    virtual wxMenu *CreatePopupMenu();

private:
    MyFrame* m_mainFrame;
};

#endif // MYTRAY_H
