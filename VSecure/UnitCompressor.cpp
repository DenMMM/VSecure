//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitCompressor.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MCompressor::Open(DWORD fccHandler)
{
    memset(&Compressor,0,sizeof(Compressor));
    Compressor.cbSize=sizeof(Compressor);
    Compressor.dwFlags=ICMF_COMPVARS_VALID;
    Compressor.fccType=ICTYPE_VIDEO;
    Compressor.fccHandler=fccHandler;
    // Открываем кодек
    Compressor.hic=ICOpen(ICTYPE_VIDEO,fccHandler,ICMODE_FASTCOMPRESS);
}
//---------------------------------------------------------------------------
void MCompressor::SetParam(LONG lKey, LONG lDataRate, LONG lQ,
    LPVOID lpSpecData, DWORD dwDataSize)
{
    // Сохраняем основные параметры для работы компрессора
    Compressor.lKey=lKey;
    Compressor.lDataRate=lDataRate;
    Compressor.lQ=lQ;
    // Если заданы специфичные для компрессора параметры, то восстанавливаем их
//    if ( lpSpecData ) ICSetState(Compressor.hic,lpSpecData,dwDataSize);
}
//---------------------------------------------------------------------------
void MCompressor::BeginCompress(BITMAPINFO *CaptureFormat)
{
    // Начинаем сжатие
    memset(&InputFormat,0,sizeof(InputFormat));
    InputFormat.bmiHeader.biSize=sizeof(InputFormat.bmiHeader);
    InputFormat.bmiHeader.biWidth=CaptureFormat->bmiHeader.biWidth;
    InputFormat.bmiHeader.biHeight=CaptureFormat->bmiHeader.biHeight;
    InputFormat.bmiHeader.biPlanes=1;
    InputFormat.bmiHeader.biBitCount=24;
    InputFormat.bmiHeader.biCompression=BI_RGB;
    ICSeqCompressFrameStart(&Compressor,&InputFormat);
}
//---------------------------------------------------------------------------
LPVOID MCompressor::Compress(LPVOID lpBits, BOOL *pfKey, LONG *plSize)
{
    return ICSeqCompressFrame(&Compressor,0,lpBits,pfKey,plSize);
}
//---------------------------------------------------------------------------
void MCompressor::EndCompress()
{
    ICSeqCompressFrameEnd(&Compressor);
}
//---------------------------------------------------------------------------
void MCompressor::Close()
{
//    ICCompressorFree(&Compressor);
    ICClose(Compressor.hic);
}
//---------------------------------------------------------------------------
LPBITMAPINFO MCompressor::GetOutputFormat()
{
    return Compressor.lpbiOut;
}
//---------------------------------------------------------------------------

