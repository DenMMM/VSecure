//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <string.h>
#include <vfw.h>
#pragma hdrstop

#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
    CaptureWindow=NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    // Заполняем список драйверов для выбора
    char Name[128], Ver[64];
    for ( WORD wDriverIndex=0; wDriverIndex<10; wDriverIndex++ )
    {
        if ( !::capGetDriverDescription(wDriverIndex,Name,128,Ver,64) ) break;
        ComboBoxDevice->Items->Add(Name);
    }
    //
    EditDir->MaxLength=MAX_PATH;
    //
    for ( int i=0; i<24; i++ )
    {
        char line[3]; sprintf(line,"%i",i);
        ComboBoxFileTimeH->Items->Add(line);
    }
    for ( int i=0; i<60; i++ )
    {
        char line[3]; sprintf(line,"%.2i",i);
        ComboBoxFileTimeM->Items->Add(line);
    }
    //
    for ( int i=1; i<=20; i++ )
    {
        char line[3];
        sprintf(line,"%i",i);
        ComboBoxFilesSize->Items->Add(line);
    }

    // Создаем окно захвата
    CaptureWindow=::capCreateCaptureWindow( "", WS_CHILD|WS_VISIBLE,
        1, 1, PanelScreen->Width-2, PanelScreen->Height-2, PanelScreen->Handle, 0);

    // Загружаем параметры
    if ( !Options.Load() ) Options.Save();

    // Инициализируем интерфейсные элементы
    if ( Options.DriverIndex<=ComboBoxDevice->Items->Count )
        ComboBoxDevice->ItemIndex=Options.DriverIndex;
    CheckBoxPreview->Checked=false;
    TrackBarRate->Position=1e6/Options.FrameRate; TrackBarRateChange(NULL);
    EditDir->Text=Options.Directory;
    ComboBoxFilesSize->ItemIndex=(Options.FilesSize/1024)-1;
    ComboBoxFileTimeH->ItemIndex=Options.NewFileTime/60;
    ComboBoxFileTimeM->ItemIndex=Options.NewFileTime%60;

    if ( Options.CodecHandler!=-1 )
    {
        HIC hic;
        ICINFO icinfo;
        hic=::ICOpen(ICTYPE_VIDEO,Options.CodecHandler,ICMODE_QUERY);
        // Если были сохранены допонительные настройки кодека, то восстанавливаем их
//        if ( Options.SpecData!=NULL )
//            ICSetState(hic,Options.SpecData,Options.SpecSize);
        // Берем описание кодека
        ICGetInfo(hic,&icinfo,sizeof(icinfo));
        ICClose(hic);
        LabelCodec->Caption=icinfo.szDescription;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ComboBoxDeviceChange(TObject *Sender)
{
    // Сохраняем номер нового драйвера
    Options.DriverIndex=ComboBoxDevice->ItemIndex;
    CheckBoxPreview->Checked=false; CheckBoxPreviewClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonSourceClick(TObject *Sender)
{
    capDlgVideoSource(CaptureWindow);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonFormatClick(TObject *Sender)
{
    capDlgVideoFormat(CaptureWindow);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TrackBarRateChange(TObject *Sender)
{
    Options.FrameRate=1e6/TrackBarRate->Position;
    LabelRate->Caption=IntToStr(TrackBarRate->Position);
    if ( CheckBoxPreview->Checked ) capPreviewRate(CaptureWindow,Options.FrameRate/1e3);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonSaveClick(TObject *Sender)
{
    Options.Save();
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonCancelClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonCompressorClick(TObject *Sender)
{
    COMPVARS pc;
    BITMAPINFO input_format;
    ICINFO icinfo;

    // Задаем формат входных данных для отбора поддерживающих его кодеков
    memset(&input_format,0,sizeof(input_format));
    input_format.bmiHeader.biSize=sizeof(input_format.bmiHeader.biSize);
    input_format.bmiHeader.biWidth=160;
    input_format.bmiHeader.biHeight=120;
    input_format.bmiHeader.biPlanes=1;
    input_format.bmiHeader.biBitCount=24;
    input_format.bmiHeader.biCompression=BI_RGB;

    // Восстанавливаем параметры для выбранного ранее кодека
/*    if ( Options.SpecData!=NULL )
    {
        HIC hic;
        if ( (hic=::ICOpen(ICTYPE_VIDEO,Options.CodecHandler,ICMODE_QUERY))!=NULL )
        {
            ICSetState(hic,Options.SpecData,Options.SpecSize);
            ::ICClose(hic);
        }
    } */

    // Выбираем кодек
    memset(&pc,0,sizeof(pc)); pc.cbSize=sizeof(pc);
    if ( !::ICCompressorChoose(Handle,ICMF_CHOOSE_DATARATE|ICMF_CHOOSE_KEYFRAME,
        &input_format,NULL,&pc,NULL) ) return;

    // Сохраняем основные параметры настройки кодека
    Options.CodecHandler=pc.fccHandler;
    Options.KeyRate=pc.lKey;
    Options.DataRate=pc.lDataRate;
    Options.Quality=pc.lQ;
    // Сохраняем специфичные для кодека настройки
/*    Options.SpecSize=ICGetStateSize(pc.hic);
    Options.SpecData=new char[Options.SpecSize];
    ICGetState(pc.hic,Options.SpecData,Options.SpecSize); */

    // Берем описание кодека
    ICGetInfo(pc.hic,&icinfo,sizeof(icinfo));
    LabelCodec->Caption=icinfo.szDescription;

    // Освобождаем ресурсы
    ICCompressorFree(&pc);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonDirClick(TObject *Sender)
{
/*    BROWSEINFO browse_info;

    browse_info.hwndOwner=Handle;
    browse_info.pidlRoot=NULL;
    browse_info.pszDisplayName=Options.Directory;
    browse_info.lpszTitle="";
    browse_info.ulFlags=0;
    browse_info.lpfn=NULL;
    browse_info.lParam=0;
    browse_info.iImage=0;
    ::SHBrowseForFolder(&browse_info); */
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::EditDirExit(TObject *Sender)
{
    strcpy(Options.Directory,EditDir->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ComboBoxFileTimeHExit(TObject *Sender)
{
    Options.NewFileTime=ComboBoxFileTimeH->ItemIndex*60+ComboBoxFileTimeM->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ComboBoxFilesSizeExit(TObject *Sender)
{
    Options.FilesSize=(ComboBoxFilesSize->ItemIndex+1)*1024;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::CheckBoxPreviewClick(TObject *Sender)
{
    if ( !CheckBoxPreview->Checked ) goto error;

    // Подключаем окно к этому драйверу
    if ( !capDriverConnect(CaptureWindow,Options.DriverIndex) )
    {
        ::MessageBox(Handle,"Не удалось подключиться к драйверу.",
            "VSetup - Ошибка", MB_OK|MB_ICONWARNING);
        goto error;
    }
    // Включаем режим просмотра и задаем параметры для него
    if ( (!capPreview(CaptureWindow,TRUE))||
        (!capPreviewScale(CaptureWindow,TRUE))||
        (!capPreviewRate(CaptureWindow,Options.FrameRate/1e3)) )
    {
        ::MessageBox(Handle,"Не удалось включить режим просмотра.",
            "VSetup - Ошибка", MB_OK|MB_ICONWARNING);
        goto error;
    }

    ButtonSource->Enabled=true;
    ButtonFormat->Enabled=true;
    return;
error:
    CheckBoxPreview->Checked=false;
    ButtonSource->Enabled=false;
    ButtonFormat->Enabled=false;
    // Отсоединяем окно от драйвера
    capDriverDisconnect(CaptureWindow);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    capDriverDisconnect(CaptureWindow);
}
//---------------------------------------------------------------------------
