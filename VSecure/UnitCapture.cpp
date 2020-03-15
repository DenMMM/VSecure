//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitCapture.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MCapture::Connect(DWORD iIndex)
{
    // Создаем окно захвата
    Window=::capCreateCaptureWindow("",0,1,1,320,240,NULL,0);
    // Подключаем окно к драйверу
    capDriverConnect(Window,iIndex);
}
//---------------------------------------------------------------------------
void MCapture::GetFormat(BITMAPINFO *CaptureFormat)
{
    // Запрашиваем формат входных данных
    capGetVideoFormat(Window,CaptureFormat,sizeof(BITMAPINFO));
}
//---------------------------------------------------------------------------
void MCapture::SetCallback(LPVOID capVideoStreamCallback)
{
    // Устанавливаем callback-функцию
    capSetCallbackOnVideoStream(Window,capVideoStreamCallback);
}
//---------------------------------------------------------------------------
void MCapture::SetParam(DWORD dwRequestMicroSecPerFrame)
{
    // Устанавливаем параметры захвата картинки
    memset(&CapParms,0,sizeof(CapParms));
    capCaptureGetSetup(Window,&CapParms,sizeof(CapParms));
    CapParms.dwRequestMicroSecPerFrame=dwRequestMicroSecPerFrame;
    CapParms.wPercentDropForError=100;
    CapParms.fYield=TRUE;
//    CapParms.fLimitEnabled=TRUE;
//    CapParms.wTimeLimit=10;
    capCaptureSetSetup(Window,&CapParms,sizeof(CapParms));
}
//---------------------------------------------------------------------------
void MCapture::Begin()
{
    capCaptureSequenceNoFile(Window);
}
//---------------------------------------------------------------------------
void MCapture::End()
{
    // Прерываем захват изображения
    capCaptureAbort(Window);

    // Ждем реального завершения захвата изображения
    CAPSTATUS status;
    memset(&status,0,sizeof(status)); status.fCapturingNow=TRUE;
    do { capGetStatus(Window,&status,sizeof(status)); ::Sleep(200); }
    while(status.fCapturingNow);
}
//---------------------------------------------------------------------------
void MCapture::Disconnect()
{
    // Отключаем Callback-функцию
    capSetCallbackOnVideoStream(Window,NULL);
    // Отключаем окно от драйвера
    capDriverDisconnect(Window);
}
//---------------------------------------------------------------------------

