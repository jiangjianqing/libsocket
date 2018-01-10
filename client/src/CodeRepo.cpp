#include "CodeRepo.h"
#include "wx/filename.h"
#include "wx/dynlib.h"
#include "wx/msgdlg.h"
#include "wx/event.h"

CodeRepo::CodeRepo()
{

}

void CodeRepo::DynamicLibraryExample()
{
    //wxString dir(wxGetWorkingDirectory());
        //wxMessageBox(dir,_T("dir"),wxOK);
    //const int MAX_PATH = 255;
    wxChar name[MAX_PATH] = {0};//vc下已包含MAX_PATH定义，会冲突
    //GetModuleFileName(0L, name, MAX_PATH);
        //wxMessageBox(name,_T("dir"),wxOK);
    wxFileName fname("path");
    wxString strPath(fname.GetPath(wxPATH_GET_VOLUME));
    wxDynamicLibrary lib;
    wxString libfile(strPath << _T("/plugins/msgplugin/msgplugin.dll"));
    lib.Load(libfile);
    if(!lib.IsLoaded()){
        wxMessageBox(libfile << _T(" load error."),_T("Error"),wxOK | wxICON_ERROR);
        return;
    }

    typedef int (*ShowMessageProc)(const wxString&);

    ShowMessageProc ShowMessage = (ShowMessageProc)lib.GetSymbol(_T("ShowMessage"));
    if(NULL == ShowMessage){
        wxMessageBox(_T("don't call the method: ShowMessag"), _T("Error"),
                wxOK | wxICON_ERROR);
        return;
    }
}

void CodeRepo::OnChar(wxKeyEvent& event)

{

  if ( wxIsalpha( event.GetKeyCode() ) )

  {

      //这些按键在可以接受的范围，所以按照正常的流程处理

      event.Skip();

   }

  else

   {

      // 这些事件不在我们可以接受的范围，所以我们不调用Skip函数

      // 由于事件表已经匹配并且没有调用Skip函数，所以事件处理过程不会

      // 再继续匹配别的事件表，而是认为事件处理已经结束

      wxBell();

   }

}
