//---------------------------------------------------------------------------
#include <windows.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma argsused
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char file_name[MAX_PATH];
    SC_HANDLE sc_manager_handle=NULL;
    SC_HANDLE service_handle=NULL;

    if ( strcmpi(lpCmdLine,"/install")==0 )
    {
        // Берем имя файла с путем к нему
        ::GetModuleFileName(NULL,file_name,MAX_PATH);
        // Открываем менеджер служб
        sc_manager_handle=::OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
        // Регистрируем службу
        service_handle=::CreateService(sc_manager_handle,"VSecure",
            "Video Secure Service",0,SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START,SERVICE_ERROR_NORMAL,file_name,
            NULL,NULL,NULL,NULL,NULL);
        //
        ::CloseServiceHandle(service_handle);
        ::CloseServiceHandle(sc_manager_handle);
    } else if ( strcmpi(lpCmdLine,"/uninstall")==0 )
    {
        // Открываем менеджер служб
        sc_manager_handle=::OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
        // Удаляем службу
        service_handle=::OpenService(sc_manager_handle,"VSecure",DELETE);
        ::DeleteService(service_handle);
        //
        ::CloseServiceHandle(service_handle);
        ::CloseServiceHandle(sc_manager_handle);
    }

    return 0;
}
//---------------------------------------------------------------------------
