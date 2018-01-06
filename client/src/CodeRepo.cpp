#include "CodeRepo.h"
#include "wx/filename.h"
#include "wx/dynlib.h"
#include "wx/msgdlg.h"

CodeRepo::CodeRepo()
{

}

void DynamicLibraryExample()
{
    //wxString dir(wxGetWorkingDirectory());
        //wxMessageBox(dir,_T("dir"),wxOK);
    int MAX_PATH = 255;
    wxChar name[MAX_PATH] = {0};
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
