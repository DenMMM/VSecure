//---------------------------------------------------------------------------
#ifndef UnitCaptureH
#define UnitCaptureH
//---------------------------------------------------------------------------
#include <windows.h>
#include <vfw.h>
//---------------------------------------------------------------------------
class MCapture
{
private:
    HWND Window;
    CAPTUREPARMS CapParms;

public:
    void Connect(DWORD iIndex);
    void GetFormat(BITMAPINFO *CaptureFormat);
    void SetCallback(LPVOID capVideoStreamCallback);
    void SetParam(DWORD dwRequestMicroSecPerFrame);
    void Begin();
    void End();
    void Disconnect();
};
//---------------------------------------------------------------------------
#endif

