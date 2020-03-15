//---------------------------------------------------------------------------
#ifndef UnitServiceH
#define UnitServiceH
//---------------------------------------------------------------------------
#include <windows.h>
#include <vfw.h>
//---------------------------------------------------------------------------
#include "UnitOptions.h"
#include "UnitCapture.h"
#include "UnitCompressor.h"
#include "UnitAVIFile.h"
//---------------------------------------------------------------------------
#define scmSTOP WM_USER+1
#define scmINTERROGATE WM_USER+2
#define scmSHUTDOWN WM_USER+3
//---------------------------------------------------------------------------
extern DWORD ServiceThreadID;
extern SERVICE_STATUS_HANDLE service_status_handle;
extern SERVICE_STATUS service_status;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
void ServiceStartProcess();
LRESULT CALLBACK capVideoStreamCallback(HWND hWnd, LPVIDEOHDR lpVHdr);
bool CheckNewFileTime(LPSYSTEMTIME lpOldTime, LPSYSTEMTIME lpNewTime, int TimeToNewFile);
int SetFileName(char *Name, char *Directory, SYSTEMTIME *Time);
bool FreeDirectory(char *Dir, int MaxSize);
bool CheckFlushTime(DWORD *OldCount, int Seconds);
//---------------------------------------------------------------------------
extern MOptions Options;
extern MCapture Capture;
extern MCompressor Compressor;
extern MAVIFile AVIFile;
extern BITMAPINFO CaptureFormat;

extern char *DataForCompress;
extern DWORD LastCaptureTime;
extern SYSTEMTIME LastFileTime;
extern char File[MAX_PATH];
extern DWORD FlushCount;
//---------------------------------------------------------------------------
#endif

