//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MOptions::MOptions()
{
    DriverIndex=-1;
    FrameRate=500;
    CodecHandler=-1;
    KeyRate=0;
    DataRate=0;
    Quality=0;
    SpecSize=0;
    SpecData=NULL;
    *Directory=0;
    NewFileTime=0;
    FilesSize=0;
}

MOptions::~MOptions()
{
    delete[] SpecData;
}

bool MOptions::Load()
{
    HKEY key=NULL;

    // Открываем раздел реестра
    if ( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Services\\VSecure\\Parameters",
        NULL,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS ) goto error;

    // Считываем параметры
    if ( !LoadValue(key,"DriverIndex",REG_DWORD_LITTLE_ENDIAN,
        (BYTE*)&DriverIndex,sizeof(DriverIndex)) ) goto error;
    if ( !LoadValue(key,"FrameRate",REG_DWORD_LITTLE_ENDIAN,
        (BYTE*)&FrameRate,sizeof(FrameRate)) ) goto error;
    if ( !LoadValue(key,"CodecHandler",REG_DWORD_LITTLE_ENDIAN,
        (BYTE*)&CodecHandler,sizeof(CodecHandler)) ) goto error;
    if ( !LoadValue(key,"KeyRate",REG_DWORD_LITTLE_ENDIAN,
        (BYTE*)&KeyRate,sizeof(KeyRate)) ) goto error;
    if ( !LoadValue(key,"DataRate",REG_DWORD_LITTLE_ENDIAN,
        (BYTE*)&DataRate,sizeof(DataRate)) ) goto error;
    if ( !LoadValue(key,"Quality",REG_DWORD_LITTLE_ENDIAN,
        (BYTE*)&Quality,sizeof(Quality)) ) goto error;
    if ( !LoadValue(key,"Directory",REG_SZ,
        (BYTE*)Directory,MAX_PATH) ) goto error;
    if ( !LoadValue(key,"NewFileTime",REG_DWORD_LITTLE_ENDIAN,
        (BYTE*)&NewFileTime,sizeof(NewFileTime)) ) goto error;
    if ( !LoadValue(key,"FilesSize",REG_DWORD_LITTLE_ENDIAN,
        (BYTE*)&FilesSize,sizeof(FilesSize)) ) goto error;

    ::RegCloseKey(key);
    return true;
error:
    if ( key!=NULL ) ::RegCloseKey(key);
    return false;
}

bool MOptions::LoadValue(HKEY hKey_, LPTSTR lpValueName_,
    DWORD dwType_, CONST BYTE *lpData_, DWORD cbData_)
{
    DWORD type, size=cbData_;

    // Считываем данные
    if ( ::RegQueryValueEx(hKey_,lpValueName_,NULL,&type,(BYTE*)lpData_,&size)!=ERROR_SUCCESS ) goto error;
    // Проверяем тип и размер на корректность
    if ( (type!=dwType_)||(size!=cbData_) ) goto error;

    return true;
error:
    return false;
}
//---------------------------------------------------------------------------

