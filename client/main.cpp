#include "wx/app.h"
#include "wx/frame.h"
#include <iostream>

using namespace std;
/*
int main()
{
    cout << "Hello World!" << endl;
    return 0;
}*/

class OurApp: public wxApp
{
    bool OnInit()
        {
        wxFrame* frame = new wxFrame(NULL, wxID_ANY, "Our First App");
                frame->Show();
        return true;
          //init
        }
};

IMPLEMENT_APP(OurApp)
