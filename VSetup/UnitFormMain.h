//---------------------------------------------------------------------------
#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#include "UnitOptions.h"
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
    TButton *ButtonSave;
    TButton *ButtonCancel;
    TPageControl *PageControl;
    TTabSheet *TabSheetDevice;
    TTabSheet *TabSheetCodec;
    TLabel *Label1;
    TComboBox *ComboBoxDevice;
    TButton *ButtonSource;
    TButton *ButtonFormat;
    TBevel *Bevel2;
    TPanel *PanelScreen;
    TCheckBox *CheckBoxPreview;
    TLabel *LabelRateText;
    TLabel *LabelRate;
    TTrackBar *TrackBarRate;
    TBevel *Bevel1;
    TLabel *Label4;
    TEdit *EditDir;
    TBevel *Bevel3;
    TButton *ButtonCompressor;
    TLabel *Label2;
    TLabel *LabelCodec;
    TBevel *Bevel4;
    TButton *ButtonDir;
    TBevel *Bevel6;
    TLabel *Label6;
    TComboBox *ComboBoxFileTimeH;
    TComboBox *ComboBoxFileTimeM;
    TLabel *Label7;
    TComboBox *ComboBoxFilesSize;
    TBevel *Bevel5;
    TBevel *Bevel7;
    TLabel *Label3;
    TLabel *Label5;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall ButtonSourceClick(TObject *Sender);
    void __fastcall ComboBoxDeviceChange(TObject *Sender);
    void __fastcall TrackBarRateChange(TObject *Sender);
    void __fastcall ButtonFormatClick(TObject *Sender);
    void __fastcall ButtonSaveClick(TObject *Sender);
    void __fastcall ButtonCancelClick(TObject *Sender);
    void __fastcall ButtonCompressorClick(TObject *Sender);
    void __fastcall ButtonDirClick(TObject *Sender);
    void __fastcall EditDirExit(TObject *Sender);
    void __fastcall ComboBoxFileTimeHExit(TObject *Sender);
    void __fastcall ComboBoxFilesSizeExit(TObject *Sender);
    void __fastcall CheckBoxPreviewClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
    HWND CaptureWindow;
    MOptions Options;
public:		// User declarations
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif

