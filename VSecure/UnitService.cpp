//---------------------------------------------------------------------------
#include <stdio.h>
#pragma hdrstop

#include "UnitService.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
DWORD ServiceThreadID;
SERVICE_STATUS_HANDLE service_status_handle;
SERVICE_STATUS service_status;
//---------------------------------------------------------------------------
MOptions Options;
MCapture Capture;
MCompressor Compressor;
MAVIFile AVIFile;
BITMAPINFO CaptureFormat;

char *DataForCompress;
DWORD LastCaptureTime;
SYSTEMTIME LastFileTime;
char File[MAX_PATH];
/*DWORD FlushCount;*/
DWORD LimitCount;

MLog Log;
//---------------------------------------------------------------------------
VOID WINAPI Handler(DWORD fdwControl)
{
    UINT Message;
    switch(fdwControl)
    {
        case SERVICE_CONTROL_STOP: Message=scmSTOP; break;
        case SERVICE_CONTROL_INTERROGATE: Message=scmINTERROGATE; break;
        case SERVICE_CONTROL_SHUTDOWN: Message=scmSHUTDOWN; break;
        default: Message=0; break;
    }
    if ( Message ) ::PostThreadMessage(ServiceThreadID,Message,0,0);
}
//---------------------------------------------------------------------------
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    // ����� ������������� ������ ��� ��� ������������� ������������ �������� �� SCM
    ServiceThreadID=::GetCurrentThreadId();
    // ������� ������� ��������� ��� ������
    ::PeekMessage(NULL,NULL,0,0,PM_NOREMOVE);
    // ������������ ���������� �������� �� SCM � ������
    service_status_handle=::RegisterServiceCtrlHandler("VSecure",&Handler);

    // ����������� SCM � ���, ��� ���� ������ ������
    service_status.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState=SERVICE_START_PENDING;
    service_status.dwControlsAccepted=//SERVICE_CONTROL_INTERROGATE+
        SERVICE_ACCEPT_STOP+SERVICE_ACCEPT_SHUTDOWN;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=1;
    service_status.dwWaitHint=5000;
    ::SetServiceStatus(service_status_handle,&service_status);

    // ��������� ��� ����� ����
    {
        char LogFile[MAX_PATH];
        ::GetModuleFileName(NULL,LogFile,MAX_PATH-strlen(".log"));
        strcat(LogFile,".log");
        Log.SetFile(LogFile);
        Log.SetSize(10*1024*1024);
    }

    Log.WriteLine("\r\n\r\n",false);  //
    Log.WriteLine("=== ������ ������ ===\r\n",true);  //

    // ��������� ���������
    Log.WriteLine("��������� ���������: ",false);  //
    if ( Options.Load() ) Log.WriteLine("���������.\r\n",false);  //
    else Log.WriteLine("�� ��������� !\r\n",false);  //

    ServiceStartProcess();

    // ����������� ���������� ������������
    Capture.Connect(Options.DriverIndex);
    Capture.SetParam(Options.FrameRate);
    Capture.SetCallback(&capVideoStreamCallback);
    Capture.GetFormat(&CaptureFormat);
    ServiceStartProcess();

    Log.WriteLine("���������� ������������ ���������.\r\n",false);  //

    // �������� ������ ��� RGB-������������� �����
    Log.WriteLine("������ ��� ��������� �����: ",false);  //
    DataForCompress=new char[CaptureFormat.bmiHeader.biWidth*
        CaptureFormat.bmiHeader.biHeight*3];
    if ( DataForCompress ) Log.WriteLine("��������.\r\n",false);  //
    else Log.WriteLine("�� �������� !\r\n",false);  //

    ServiceStartProcess();

    // ��������� ����������
    Compressor.Open(Options.CodecHandler);
    // ������ �������� ��������� ��� �����������
    Compressor.SetParam(Options.KeyRate,Options.DataRate,Options.Quality,
        Options.SpecData,Options.SpecSize);
    ServiceStartProcess();

    Log.WriteLine("���������� ��������.\r\n",false);  //

    AVIFileInit();

    // ������� ���������� �� ������ ������
    Log.WriteLine("���������� \"",false);  //
    Log.WriteLine(Options.Directory,false);  //
    Log.WriteLine("\" �� ������ ������: ",false);  //
    if ( FreeDirectory(Options.Directory,Options.FilesSize) ) Log.WriteLine("�������.\r\n",false);  //
    else Log.WriteLine("�� ������� !\r\n",false);  //
    // �������������� ���������� � ������ ������������������ ������
    Log.WriteLine("������: ",false);  //
    if ( Compressor.BeginCompress(&CaptureFormat) ) Log.WriteLine("������.\r\n",false);  //
    else Log.WriteLine("�� ������ !\r\n",false);  //
    // ����� ������� ����� ��� �������� �����
    CheckNewFileTime(&LastFileTime,-1);
    // ��������� ����� ��� �����
    SetFileName(File,Options.Directory,&LastFileTime);
    Log.WriteLine("���� \"",false);  //
    Log.WriteLine(File,false);
    Log.WriteLine("\" ",false);  //
    // ������� ����
    if ( AVIFile.Create(File,Options.CodecHandler,Options.Quality,
        Compressor.GetOutputFormat(),Options.FrameRate) ) Log.WriteLine("������.\r\n",false);  //
    else Log.WriteLine("�� ������ !\r\n",false);  //
    // �������� ������ ��������
    LastCaptureTime=-1; Capture.Begin();
/*    // ���������� ������� ������������ �����
    CheckFlushTime(&FlushCount,0); */
    // ���������� ������� ��������� ����� �����
    CheckFlushTime(&LimitCount,0);

    // ����������� SCM �� �������� ������� ������
    service_status.dwCurrentState=SERVICE_RUNNING;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

//    int i=0;

    Log.WriteLine("=== ������ �������� ===\r\n\r\n",true);  //

    while(true)
    {
        // ��������� �� ���� �� �������� ����� ���� �� ������������� � ���������� �������
        // ��� ��-�� ���������� ���������� ������������ �� �������
        if ( CheckNewFileTime(&LastFileTime,Options.NewFileTime)||
            CheckFlushTime(&LimitCount,25*60*60) )
        {
            Log.WriteLine("�������� ������ ����� �����.\r\n",true);  //
            // ������������� ������ ��������
            Capture.End();
            // ��������� ������
            Compressor.EndCompress();
            // ��������� AVI-����
            Log.WriteLine("AVI-����: ",false);  //
            if ( AVIFile.Close() ) Log.WriteLine("������.\r\n",false);  //
            else Log.WriteLine("�� ������ !\r\n",false);  //
            // ������� ���������� �� ������ ������
            Log.WriteLine("���������� \"",false);  //
            Log.WriteLine(Options.Directory,false);  //
            Log.WriteLine("\" �� ������ ������: ",false);  //
            if ( FreeDirectory(Options.Directory,Options.FilesSize) ) Log.WriteLine("�������.\r\n",false);  //
            else Log.WriteLine("�� ������� !\r\n",false);  //
            // �������������� ���������� � ������ ������������������ ������
            Log.WriteLine("������: ",false);  //
            if ( Compressor.BeginCompress(&CaptureFormat) ) Log.WriteLine("������.\r\n",false);  //
            else Log.WriteLine("�� ������ !\r\n",false);  //
            // ����� ������� ����� ��� �������� �����
            CheckNewFileTime(&LastFileTime,-1);
            // ��������� ����� ��� �����
            SetFileName(File,Options.Directory,&LastFileTime);
            Log.WriteLine("��� ����� ��� ������: ",false);  //
            Log.WriteLine(File,false); Log.WriteLine("\r\n",false);  //
            // ������� ����
            Log.WriteLine("AVI-����: ",false);  //
            if ( AVIFile.Create(File,Options.CodecHandler,Options.Quality,
                Compressor.GetOutputFormat(),Options.FrameRate) )  Log.WriteLine("������.\r\n",false);  //
            else  Log.WriteLine("�� ������ !\r\n",false);  //
            // �������� ������ ��������
            LastCaptureTime=-1; Capture.Begin();
/*            // ���������� ������� ������������ �����
            CheckFlushTime(&FlushCount,0); */
            // ���������� ������� ��������� ����� �����
            CheckFlushTime(&LimitCount,0);
        } /*else if ( CheckFlushTime(&FlushCount,2*60) )
        {
            // ������������� ������ ��������, ��������� ����
            Capture.End(); AVIFile.Close();
            // ��������� ���� �����
            AVIFile.Open(File,Options.CodecHandler);
            // �������� ������ ��������
            LastCaptureTime=-1; Capture.Begin();
            // ���������� ������� ������������ �����
            CheckFlushTime(&FlushCount,0);
        }*/

        // ��������� ������� ���������
        MSG Message;
        if ( ::PeekMessage(&Message,NULL,0,0,PM_REMOVE) )
        {
            if ( (Message.message==scmSTOP)||
                (Message.message==scmSHUTDOWN) ) break;
//            if ( Message.message==scmINTERROGATE ) ;
        }
        ::Sleep(200);
//        if ( (++i)>=(120*5) ) break;
    }

    service_status.dwCurrentState=SERVICE_STOP_PENDING;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

    Log.WriteLine("\r\n",false);  //
    Log.WriteLine("=== ������� ������ ===\r\n",true);  //

    // ������������� ������ �������� � ����������� �������, ��������� � �������������
    Capture.End(); Capture.Disconnect(); delete[] DataForCompress;
    ServiceStartProcess();
    //
    Compressor.EndCompress(); Compressor.Close();
    ServiceStartProcess();
    // ��������� AVI-����
    Log.WriteLine("AVI-����: ",false);  //
    if ( AVIFile.Close() ) Log.WriteLine("������.\r\n",false);  //
    else Log.WriteLine("�� ������ !\r\n",false);  //
    AVIFileExit();

    // ����������� SCM �� �������� �������� ������
    service_status.dwCurrentState=SERVICE_STOPPED;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

    Log.WriteLine("=== ������ ����������� ===\r\n",true);  //
}
//---------------------------------------------------------------------------
void ServiceStartProcess()
{
    service_status.dwCheckPoint++;
    service_status.dwWaitHint=5000;
    ::SetServiceStatus(service_status_handle,&service_status);
}
//---------------------------------------------------------------------------
bool CheckNewFileTime(LPSYSTEMTIME lpOldTime, int TimeToNewFile)
{
    SYSTEMTIME ssCurrentTime;
    int iTargetTime=TimeToNewFile*60*1000;

    // ���������� ������� ��������� �����
    ::GetLocalTime(&ssCurrentTime);
    //
    if ( TimeToNewFile==-1 )
        { memcpy(lpOldTime,&ssCurrentTime,sizeof(SYSTEMTIME)); return false; }
    // ���������� ������� ����� � �������
    if ( GetTimeOfSystemTime(&ssCurrentTime)<iTargetTime ) return false;
    // ���������� ����� �������� ���������� ����� � ������� ��������
    if ( (ssCurrentTime.wYear==lpOldTime->wYear)&&
        (ssCurrentTime.wMonth==lpOldTime->wMonth)&&
        (ssCurrentTime.wDay==lpOldTime->wDay)&&
        (GetTimeOfSystemTime(lpOldTime)>=iTargetTime) ) return false;

    return true;
}
//---------------------------------------------------------------------------
int GetTimeOfSystemTime(LPSYSTEMTIME lpTime)
{
    return lpTime->wHour*(60*60*1000)+lpTime->wMinute*(60*1000)+
        lpTime->wSecond*(1000)+lpTime->wMilliseconds;
}
//---------------------------------------------------------------------------
int SetFileName(char *Name, char *Directory, SYSTEMTIME *Time)
{
    return sprintf(Name, "%s%u.%02u.%02u - %02u.%02u.%02u.AVI", Directory,
        Time->wYear, Time->wMonth, Time->wDay, Time->wHour, Time->wMinute, Time->wSecond);
/*
    int NameSize;
    strcpy(Name,Directory); NameSize=strlen(Name);
//    NameSize+=::GetDateFormat(LOCALE_SYSTEM_DEFAULT,0,Time,"yyyy, MMMM dd (ddd)",Name+NameSize,80);
    NameSize+=::GetTimeFormat(LOCALE_SYSTEM_DEFAULT,0,Time," - HH.mm.ss",Name+NameSize,80);
    return NameSize; */
}
//---------------------------------------------------------------------------
bool FreeDirectory(char *Dir, int MaxSize)
{
    char SearchName[MAX_PATH], OldestName[MAX_PATH], DelName[MAX_PATH];
    HANDLE hSearchFile;
    WIN32_FIND_DATA FindData;
    FILETIME OldestTime;
    __int64 AllSize;

    // ��������� ������ ��� ������ ������
    sprintf(SearchName,"%s*.AVI",Dir);

    while(true)
    {
        // �������� ����� ������
        hSearchFile=::FindFirstFile(SearchName,&FindData);
        if ( hSearchFile==INVALID_HANDLE_VALUE ) break;
        // ��������� ��������� ������� ����������� ����� ��� ������������ ���������
        OldestTime=FindData.ftCreationTime;
        strcpy(OldestName,FindData.cFileName);
        //
        AllSize=(((__int64)FindData.nFileSizeHigh)*MAXDWORD)+FindData.nFileSizeLow;
        // ���� ��������� ����� � ��������� ����� �� ��������
        while(::FindNextFile(hSearchFile,&FindData))
        {
            // ���� ����� �������� ���������� ����� ������ ������������,
            // �� ��������� ������
            if ( ::CompareFileTime(&FindData.ftCreationTime,&OldestTime)==-1 )
            {
                OldestTime=FindData.ftCreationTime;
                strcpy(OldestName,FindData.cFileName);
            }
            //
            AllSize+=(((__int64)FindData.nFileSizeHigh)*MAXDWORD)+FindData.nFileSizeLow;
        }
        // ��������� �����
        ::FindClose(hSearchFile);

        //
        if ( (MaxSize*1024i64*1024)>=AllSize ) break;
        strcpy(DelName,Dir); strcat(DelName,OldestName);
        if ( !::DeleteFile(DelName) ) return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CheckFlushTime(DWORD *OldCount, int Seconds)
{
    DWORD Count;

    //
    Count=::GetTickCount();
    if ( Seconds==0 ) { *OldCount=Count; return false; }
    //
    if ( Count<(*OldCount+Seconds*1000) ) return false;

    return true;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK capVideoStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr)
{
    static bool NotWrite=false;
    static bool NotCompress=false;
    LPVOID lpCompressData;
    BOOL fKey=TRUE;
    LONG lSize=lpVHdr->dwBytesUsed/2*3;

    if ( (lpVHdr->dwTimeCaptured-LastCaptureTime)<(Options.FrameRate/1000) ) return true;
    LastCaptureTime=lpVHdr->dwTimeCaptured;

    if ( CaptureFormat.bmiHeader.biBitCount==16 )
    {
        for ( int i=0, j=lpVHdr->dwBytesUsed/2; i<j; i++ )
        {
            WORD color;
            color=((WORD*)lpVHdr->lpData)[i];
            DataForCompress[i*3+0]=(color<<3)&0xF8;
            DataForCompress[i*3+1]=(color>>2)&0xF8;
            DataForCompress[i*3+2]=(color>>7)&0xF8;
        }
    }

    // ������� RGB-������������� �����
    if ( (lpCompressData=Compressor.Compress(DataForCompress,&fKey,&lSize))!=NULL )
    {
        if ( NotCompress ) Log.WriteLine("���� ����.\r\n",true);  //
        NotCompress=false;
    } else
    {
        if ( !NotCompress ) Log.WriteLine("���� �� ���� !\r\n",true);  //
        NotCompress=true;
    }

    if ( lpCompressData==NULL ) return true;

    // ���������� ������ ���� � ����
    if ( AVIFile.Write(lpCompressData,lSize,fKey) )
    {
        if ( NotWrite ) Log.WriteLine("���� ������� � ����.\r\n",true);  //
        NotWrite=false;
    } else
    {
        if ( !NotWrite ) Log.WriteLine("���� �� ������� � ���� !\r\n",true);  //
        NotWrite=true;
    }

    return true;
}
//---------------------------------------------------------------------------

