//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("VSetup.res");
USEFORM("UnitFormMain.cpp", FormMain);
USEUNIT("UnitOptions.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "VSecure Setup";
         Application->CreateForm(__classid(TFormMain), &FormMain);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
