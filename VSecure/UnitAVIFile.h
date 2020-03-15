//---------------------------------------------------------------------------
#ifndef UnitAVIFileH
#define UnitAVIFileH
//---------------------------------------------------------------------------
#include <windows.h>
#include <vfw.h>
//---------------------------------------------------------------------------
class MAVIFile
{
private:
    PAVIFILE pAVIFile;
    PAVISTREAM pAVIStream;
    LONG lAVIPos;
    AVISTREAMINFO streaminfo;

public:
    bool Create(LPCTSTR szFile, DWORD fccHandler, DWORD dwQuality,
        LPBITMAPINFO lpbiOut, DWORD dwRate);
    /*bool Open(LPCTSTR szFile, DWORD fccHandler);*/
    bool Write(LPVOID lpData, LONG lDataSize, BOOL fKeyFrame);
    bool Close();
};
//---------------------------------------------------------------------------
#endif

