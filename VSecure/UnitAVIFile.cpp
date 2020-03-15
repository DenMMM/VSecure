//---------------------------------------------------------------------------
#pragma hdrstop

#include "UnitAVIFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool MAVIFile::Create(LPCTSTR szFile, DWORD fccHandler, DWORD dwQuality,
    LPBITMAPINFO lpbiOut, DWORD dwRate)
{
    // ������� ����
    if ( AVIFileOpen(&pAVIFile,szFile,OF_CREATE|OF_WRITE|OF_SHARE_EXCLUSIVE,NULL) ) return false;
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
    if ( AVIFileCreateStream(pAVIFile,&pAVIStream,&streaminfo) ) return false;
    // ������������� ������ ������
    if ( AVIStreamSetFormat(pAVIStream,lAVIPos,lpbiOut,sizeof(BITMAPINFO)) ) return false;

    return true;
}
//---------------------------------------------------------------------------
/*bool MAVIFile::Open(LPCTSTR szFile, DWORD fccHandler)
{
    // ��������� ����
    AVIFileOpen(&pAVIFile,szFile,OF_READWRITE|OF_SHARE_EXCLUSIVE,NULL);
    // ��������� ����� � �����
//    AVIFileGetStream(pAVIFile,&pAVIStream,streamtypeVIDEO,fccHandler);
    AVIFileGetStream(pAVIFile,&pAVIStream,streamtypeVIDEO,0);
    // ���������� ���������� ������� � �����
    lAVIPos=AVIStreamLength(pAVIStream);

    return true;
}*/
//---------------------------------------------------------------------------
bool MAVIFile::Write(LPVOID lpData, LONG lDataSize, BOOL fKeyFrame)
{
    if ( AVIStreamWrite(pAVIStream,lAVIPos,1,lpData,lDataSize,
        fKeyFrame?AVIIF_KEYFRAME:0,NULL,NULL) ) return false;
    lAVIPos++;
    return true;
}
//---------------------------------------------------------------------------
bool MAVIFile::Close()
{
    // ��������� ����� � �����
    AVIStreamRelease(pAVIStream);
    // ��������� ����
    AVIFileRelease(pAVIFile);

    return true;
}
//---------------------------------------------------------------------------

