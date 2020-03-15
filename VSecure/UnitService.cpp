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
    // Берем идентификатор потока для его использования обработчиком запросов от SCM
    ServiceThreadID=::GetCurrentThreadId();
    // Создаем очередь сообщений для потока
    ::PeekMessage(NULL,NULL,0,0,PM_NOREMOVE);
    // Регистрируем обработчик запросов от SCM к службе
    service_status_handle=::RegisterServiceCtrlHandler("VSecure",&Handler);

    // Информируем SCM о том, что идет запуск службы
    service_status.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState=SERVICE_START_PENDING;
    service_status.dwControlsAccepted=//SERVICE_CONTROL_INTERROGATE+
        SERVICE_ACCEPT_STOP+SERVICE_ACCEPT_SHUTDOWN;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=1;
    service_status.dwWaitHint=5000;
    ::SetServiceStatus(service_status_handle,&service_status);

    // Формируем имя файла лога
    {
        char LogFile[MAX_PATH];
        ::GetModuleFileName(NULL,LogFile,MAX_PATH-strlen(".log"));
        strcat(LogFile,".log");
        Log.SetFile(LogFile);
        Log.SetSize(10*1024*1024);
    }

    Log.WriteLine("\r\n\r\n",false);  //
    Log.WriteLine("=== Запуск службы ===\r\n",true);  //

    // Загружаем настройки
    Log.WriteLine("Загружаем настройки: ",false);  //
    if ( Options.Load() ) Log.WriteLine("выполнено.\r\n",false);  //
    else Log.WriteLine("НЕ выполнено !\r\n",false);  //

    ServiceStartProcess();

    // Настраиваем устройство видеозахвата
    Capture.Connect(Options.DriverIndex);
    Capture.SetParam(Options.FrameRate);
    Capture.SetCallback(&capVideoStreamCallback);
    Capture.GetFormat(&CaptureFormat);
    ServiceStartProcess();

    Log.WriteLine("Устройство видеозахвата настроено.\r\n",false);  //

    // Выделяем память под RGB-представление кадра
    Log.WriteLine("Память для обработки кадра: ",false);  //
    DataForCompress=new char[CaptureFormat.bmiHeader.biWidth*
        CaptureFormat.bmiHeader.biHeight*3];
    if ( DataForCompress ) Log.WriteLine("выделена.\r\n",false);  //
    else Log.WriteLine("НЕ выделена !\r\n",false);  //

    ServiceStartProcess();

    // Открываем компрессор
    Compressor.Open(Options.CodecHandler);
    // Задаем основные параметры для компрессора
    Compressor.SetParam(Options.KeyRate,Options.DataRate,Options.Quality,
        Options.SpecData,Options.SpecSize);
    ServiceStartProcess();

    Log.WriteLine("Компрессор настроен.\r\n",false);  //

    AVIFileInit();

    // Очищаем директорию от лишних файлов
    Log.WriteLine("Директория \"",false);  //
    Log.WriteLine(Options.Directory,false);  //
    Log.WriteLine("\" от лишних файлов: ",false);  //
    if ( FreeDirectory(Options.Directory,Options.FilesSize) ) Log.WriteLine("очищена.\r\n",false);  //
    else Log.WriteLine("НЕ очищена !\r\n",false);  //
    // Подготавливаем компрессор к сжатию последовательности кадров
    Log.WriteLine("Сжатие: ",false);  //
    if ( Compressor.BeginCompress(&CaptureFormat) ) Log.WriteLine("начато.\r\n",false);  //
    else Log.WriteLine("НЕ начато !\r\n",false);  //
    // Берем текущее время для создания файла
    CheckNewFileTime(&LastFileTime,-1);
    // Формируем новое имя файла
    SetFileName(File,Options.Directory,&LastFileTime);
    Log.WriteLine("Файл \"",false);  //
    Log.WriteLine(File,false);
    Log.WriteLine("\" ",false);  //
    // Создаем файл
    if ( AVIFile.Create(File,Options.CodecHandler,Options.Quality,
        Compressor.GetOutputFormat(),Options.FrameRate) ) Log.WriteLine("создан.\r\n",false);  //
    else Log.WriteLine("НЕ создан !\r\n",false);  //
    // Начинаем захват картинки
    LastCaptureTime=-1; Capture.Begin();
/*    // Сбрасываем счетчик автозакрытия файла
    CheckFlushTime(&FlushCount,0); */
    // Сбрасываем счетчик аварийной смены файла
    CheckFlushTime(&LimitCount,0);

    // Информируем SCM об успешном запуске службы
    service_status.dwCurrentState=SERVICE_RUNNING;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

//    int i=0;

    Log.WriteLine("=== Служба запущена ===\r\n\r\n",true);  //

    while(true)
    {
        // Проверяем не пора ли заводить новый файл по выставленному в настройках времени
        // или из-за превышения предельной длительности по времени
        if ( CheckNewFileTime(&LastFileTime,Options.NewFileTime)||
            CheckFlushTime(&LimitCount,25*60*60) )
        {
            Log.WriteLine("Сработал таймер смены файла.\r\n",true);  //
            // Останавливаем захват картинки
            Capture.End();
            // Завершаем сжатие
            Compressor.EndCompress();
            // Закрываем AVI-файл
            Log.WriteLine("AVI-файл: ",false);  //
            if ( AVIFile.Close() ) Log.WriteLine("закрыт.\r\n",false);  //
            else Log.WriteLine("НЕ закрыт !\r\n",false);  //
            // Очищаем директорию от лишних файлов
            Log.WriteLine("Директория \"",false);  //
            Log.WriteLine(Options.Directory,false);  //
            Log.WriteLine("\" от лишних файлов: ",false);  //
            if ( FreeDirectory(Options.Directory,Options.FilesSize) ) Log.WriteLine("очищена.\r\n",false);  //
            else Log.WriteLine("НЕ очищена !\r\n",false);  //
            // Подготавливаем компрессор к сжатию последовательности кадров
            Log.WriteLine("Сжатие: ",false);  //
            if ( Compressor.BeginCompress(&CaptureFormat) ) Log.WriteLine("начато.\r\n",false);  //
            else Log.WriteLine("НЕ начато !\r\n",false);  //
            // Берем текущее время для создания файла
            CheckNewFileTime(&LastFileTime,-1);
            // Формируем новое имя файла
            SetFileName(File,Options.Directory,&LastFileTime);
            Log.WriteLine("Имя файла для записи: ",false);  //
            Log.WriteLine(File,false); Log.WriteLine("\r\n",false);  //
            // Создаем файл
            Log.WriteLine("AVI-файл: ",false);  //
            if ( AVIFile.Create(File,Options.CodecHandler,Options.Quality,
                Compressor.GetOutputFormat(),Options.FrameRate) )  Log.WriteLine("создан.\r\n",false);  //
            else  Log.WriteLine("НЕ создан !\r\n",false);  //
            // Начинаем захват картинки
            LastCaptureTime=-1; Capture.Begin();
/*            // Сбрасываем счетчик автозакрытия файла
            CheckFlushTime(&FlushCount,0); */
            // Сбрасываем счетчик аварийной смены файла
            CheckFlushTime(&LimitCount,0);
        } /*else if ( CheckFlushTime(&FlushCount,2*60) )
        {
            // Останавливаем захват картинки, закрываем файл
            Capture.End(); AVIFile.Close();
            // Открываем файл снова
            AVIFile.Open(File,Options.CodecHandler);
            // Начинаем захват картинки
            LastCaptureTime=-1; Capture.Begin();
            // Сбрасываем счетчик автозакрытия файла
            CheckFlushTime(&FlushCount,0);
        }*/

        // Проверяем очередь сообщений
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
    Log.WriteLine("=== Останов службы ===\r\n",true);  //

    // Останавливаем захват картинки и освобождаем ресурсы, связанные с видеозахватом
    Capture.End(); Capture.Disconnect(); delete[] DataForCompress;
    ServiceStartProcess();
    //
    Compressor.EndCompress(); Compressor.Close();
    ServiceStartProcess();
    // Закрываем AVI-файл
    Log.WriteLine("AVI-файл: ",false);  //
    if ( AVIFile.Close() ) Log.WriteLine("закрыт.\r\n",false);  //
    else Log.WriteLine("НЕ закрыт !\r\n",false);  //
    AVIFileExit();

    // Информируем SCM об успешном останове службы
    service_status.dwCurrentState=SERVICE_STOPPED;
    service_status.dwWin32ExitCode=NO_ERROR;
    service_status.dwServiceSpecificExitCode=0;
    service_status.dwCheckPoint=0;
    service_status.dwWaitHint=0;
    ::SetServiceStatus(service_status_handle,&service_status);

    Log.WriteLine("=== Служба остановлена ===\r\n",true);  //
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

    // Определяем текущее локальное время
    ::GetLocalTime(&ssCurrentTime);
    //
    if ( TimeToNewFile==-1 )
        { memcpy(lpOldTime,&ssCurrentTime,sizeof(SYSTEMTIME)); return false; }
    // Сравниваем текущее время с целевым
    if ( GetTimeOfSystemTime(&ssCurrentTime)<iTargetTime ) return false;
    // Сравниваем время создания последнего файла с целевым временем
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

    // Формируем шаблон для поиска файлов
    sprintf(SearchName,"%s*.AVI",Dir);

    while(true)
    {
        // Начинаем поиск файлов
        hSearchFile=::FindFirstFile(SearchName,&FindData);
        if ( hSearchFile==INVALID_HANDLE_VALUE ) break;
        // Сохраняем параметры первого попавшегося файла для последующего сравнения
        OldestTime=FindData.ftCreationTime;
        strcpy(OldestName,FindData.cFileName);
        //
        AllSize=(((__int64)FindData.nFileSizeHigh)*MAXDWORD)+FindData.nFileSizeLow;
        // Ищем остальные файлы и проверяем время их создания
        while(::FindNextFile(hSearchFile,&FindData))
        {
            // Если время создания найденного файла меньше сохраненного,
            // то обновляем данные
            if ( ::CompareFileTime(&FindData.ftCreationTime,&OldestTime)==-1 )
            {
                OldestTime=FindData.ftCreationTime;
                strcpy(OldestName,FindData.cFileName);
            }
            //
            AllSize+=(((__int64)FindData.nFileSizeHigh)*MAXDWORD)+FindData.nFileSizeLow;
        }
        // Завершаем поиск
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

    // Сжимаем RGB-представление кадра
    if ( (lpCompressData=Compressor.Compress(DataForCompress,&fKey,&lSize))!=NULL )
    {
        if ( NotCompress ) Log.WriteLine("Кадр сжат.\r\n",true);  //
        NotCompress=false;
    } else
    {
        if ( !NotCompress ) Log.WriteLine("Кадр НЕ сжат !\r\n",true);  //
        NotCompress=true;
    }

    if ( lpCompressData==NULL ) return true;

    // Записываем сжатый кадр в файл
    if ( AVIFile.Write(lpCompressData,lSize,fKey) )
    {
        if ( NotWrite ) Log.WriteLine("Кадр записан в файл.\r\n",true);  //
        NotWrite=false;
    } else
    {
        if ( !NotWrite ) Log.WriteLine("Кадр НЕ записан в файл !\r\n",true);  //
        NotWrite=true;
    }

    return true;
}
//---------------------------------------------------------------------------

