//---------------------------------------------------------------------------
#include <windows.h>
#include <string.h>
#include <stdio.h>
#pragma hdrstop

#include "UnitLog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
MLog::MLog()
{
    File=NULL;
    MaxSize=0;
}

MLog::~MLog()
{
    delete[] File;
}


bool MLog::SetFile(char *FileName_)
{
    delete[] File; File=NULL;

    int length=strlen(FileName_)+1;
    File=new char[length]; if ( File==NULL ) return false;
    memcpy(File,FileName_,length);

    return true;
}

bool MLog::SetSize(int Size_)
{
    MaxSize=Size_;
}

bool MLog::WriteLine(char *Text_, bool Time_)
{
    if ( File==NULL ) return true;

    HANDLE hFile=NULL;
    DWORD filesize, length, written;

    // Открываем/создаем файл лога
    hFile=::CreateFile(File,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_WRITE_THROUGH,NULL);
    if ( hFile==INVALID_HANDLE_VALUE ) goto error;

    // Переводим текущую позицию в файле к его концу
    filesize=::SetFilePointer(hFile,0,NULL,FILE_END);
    if ( filesize==0xFFFFFFFF ) goto error;
    // Проверяем не превышен ли допустимый размер файла
    if ( filesize>=MaxSize )
    {
        // Начинаем писать файл с начала
        if ( (::SetFilePointer(hFile,0,NULL,FILE_BEGIN)==0xFFFFFFFF)||
            (::SetEndOfFile(hFile)==0) ) goto error;
    }

    // Добавляем метку о времени
    if ( Time_ )
    {
        SYSTEMTIME ssTime; char time[40+1];
        // Формируем строку со временем
/*        ::GetLocalTime(&ssTime); time[0]=0;
        length=::GetDateFormat(LOCALE_SYSTEM_DEFAULT,0,&ssTime,"yyyy.MM.dd",time,80);
        length+=::GetTimeFormat(LOCALE_SYSTEM_DEFAULT,0,&ssTime," - HH.mm.ss,  ",time+length,80); */
        ::GetLocalTime(&ssTime);
        length=sprintf(time, "%u.%02u.%02u - %02u.%02u.%02u,  ", ssTime.wYear, ssTime.wMonth,
            ssTime.wDay, ssTime.wHour, ssTime.wMinute, ssTime.wSecond);
        // Записываем время в файл
        if ( (::WriteFile(hFile,time,length,&written,NULL)==0)||
            (written!=length) ) goto error;
    }

    // Записываем данные в файл
    length=strlen(Text_);
    if ( (::WriteFile(hFile,Text_,length,&written,NULL)==0)||
        (written!=length) ) goto error;

    return ::CloseHandle(hFile);
error:
    if ( (hFile!=NULL)&&(hFile!=INVALID_HANDLE_VALUE) ) ::CloseHandle(hFile);
    return false;
}

//---------------------------------------------------------------------------

