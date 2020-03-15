//---------------------------------------------------------------------------
#ifndef UnitCompressorH
#define UnitCompressorH
//---------------------------------------------------------------------------
#include <windows.h>
#include <vfw.h>
//---------------------------------------------------------------------------
class MCompressor
{
private:
    COMPVARS Compressor;
    BITMAPINFO InputFormat;
public:
    void Open(DWORD fccHandler);
    void SetParam(LONG lKey, LONG lDataRate, LONG lQ, LPVOID lpSpecData, DWORD dwDataSize);
    bool BeginCompress(BITMAPINFO *CaptureFormat);
    LPVOID Compress(LPVOID lpBits, BOOL *pfKey, LONG *plSize);
    void EndCompress();
    void Close();

    LPBITMAPINFO GetOutputFormat();
};
//---------------------------------------------------------------------------
#endif

