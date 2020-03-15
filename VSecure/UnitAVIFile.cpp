//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitAVIFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void MAVIFile::Create(LPCTSTR szFile, DWORD fccHandler, DWORD dwQuality,
    LPBITMAPINFO lpbiOut, DWORD dwRate)
{
    // ������� ����
    AVIFileOpen(&pAVIFile,szFile,OF_CREATE|OF_WRITE|OF_SHARE_EXCLUSIVE,NULL);
    lAVIPos=0;
    // ��������� ���������, ������������ �������� ��������� ����������� ������
    memset(&streaminfo,0,sizeof(streaminfo));
    streaminfo.fccType=streamtypeVIDEO;
    streaminfo.fccHandler=fccHandler;
    streaminfo.dwScale=1;
    streaminfo.dwRate=1e6/dwRate;
    streaminfo.dwQuality=dwQuality;
    streaminfo.rcFrame.right=lpbiOut->bmiHeader.biWidth;
    streaminfo.rcFrame.bottom=lpbiOut->bmiHeader.biHeight;
    // ������� ����� � �����
    AVIFileCreateStream(pAVIFile,&pAVIStream,&streaminfo);
    // ������������� ������ ������
    AVIStreamSetFormat(pAVIStream,lAVIPos,lpbiOut,sizeof(BITMAPINFO));
}
//---------------------------------------------------------------------------
void MAVIFile::Open(LPCTSTR szFile, DWORD fccHandler)
{
    // ��������� ����
    AVIFileOpen(&pAVIFile,szFile,OF_READWRITE|OF_SHARE_EXCLUSIVE,NULL);
    // ��������� ����� � �����
//    AVIFileGetStream(pAVIFile,&pAVIStream,streamtypeVIDEO,fccHandler);
    AVIFileGetStream(pAVIFile,&pAVIStream,streamtypeVIDEO,0);
    // ���������� ���������� ������� � �����
    lAVIPos=AVIStreamLength(pAVIStream);
}
//---------------------------------------------------------------------------
void MAVIFile::Write(LPVOID lpData, LONG lDataSize, BOOL fKeyFrame)
{
    AVIStreamWrite(pAVIStream,lAVIPos,1,lpData,lDataSize,
        fKeyFrame?AVIIF_KEYFRAME:0,NULL,NULL);
    lAVIPos++;
}
//---------------------------------------------------------------------------
void MAVIFile::Close()
{
    // ��������� ����� � �����
    AVIStreamRelease(pAVIStream);
    // ��������� ����
    AVIFileRelease(pAVIFile);
}
//---------------------------------------------------------------------------

