//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitCapture.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MCapture::Connect(DWORD iIndex)
{
    // ������� ���� �������
    Window=::capCreateCaptureWindow("",0,1,1,320,240,NULL,0);
    // ���������� ���� � ��������
    capDriverConnect(Window,iIndex);
}
//---------------------------------------------------------------------------
void MCapture::GetFormat(BITMAPINFO *CaptureFormat)
{
    // ����������� ������ ������� ������
    capGetVideoFormat(Window,CaptureFormat,sizeof(BITMAPINFO));
}
//---------------------------------------------------------------------------
void MCapture::SetCallback(LPVOID capVideoStreamCallback)
{
    // ������������� callback-�������
    capSetCallbackOnVideoStream(Window,capVideoStreamCallback);
}
//---------------------------------------------------------------------------
void MCapture::SetParam(DWORD dwRequestMicroSecPerFrame)
{
    // ������������� ��������� ������� ��������
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
    // ��������� ������ �����������
    capCaptureAbort(Window);

    // ���� ��������� ���������� ������� �����������
    CAPSTATUS status;
    memset(&status,0,sizeof(status)); status.fCapturingNow=TRUE;
    do { capGetStatus(Window,&status,sizeof(status)); ::Sleep(200); }
    while(status.fCapturingNow);
}
//---------------------------------------------------------------------------
void MCapture::Disconnect()
{
    // ��������� Callback-�������
    capSetCallbackOnVideoStream(Window,NULL);
    // ��������� ���� �� ��������
    capDriverDisconnect(Window);
}
//---------------------------------------------------------------------------

