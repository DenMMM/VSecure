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
    void Create(LPCTSTR szFile, DWORD fccHandler, DWORD dwQuality,
        LPBITMAPINFO lpbiOut, DWORD dwRate);
    void Open(LPCTSTR szFile, DWORD fccHandler);
    void Write(LPVOID lpData, LONG lDataSize, BOOL fKeyFrame);
    void Close();
};
//---------------------------------------------------------------------------
#endif

