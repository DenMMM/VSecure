object FormMain: TFormMain
  Left = 256
  Top = 180
  ActiveControl = ComboBoxDevice
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Настройка VSecure'
  ClientHeight = 398
  ClientWidth = 507
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonSave: TButton
    Left = 312
    Top = 368
    Width = 85
    Height = 25
    Caption = 'Сохранить'
    TabOrder = 1
    OnClick = ButtonSaveClick
  end
  object ButtonCancel: TButton
    Left = 404
    Top = 368
    Width = 85
    Height = 25
    Caption = 'Отмена'
    TabOrder = 2
    OnClick = ButtonCancelClick
  end
  object PageControl: TPageControl
    Left = 4
    Top = 4
    Width = 497
    Height = 357
    ActivePage = TabSheetDevice
    TabOrder = 0
    object TabSheetDevice: TTabSheet
      Caption = 'Параметры устройства'
      object Bevel2: TBevel
        Left = 4
        Top = 4
        Width = 481
        Height = 321
        Shape = bsFrame
      end
      object Label1: TLabel
        Left = 22
        Top = 272
        Width = 63
        Height = 13
        Caption = 'Устройство:'
      end
      object LabelRateText: TLabel
        Left = 63
        Top = 300
        Width = 126
        Height = 13
        Alignment = taRightJustify
        Caption = 'Число кадров в секунду:'
      end
      object LabelRate: TLabel
        Left = 424
        Top = 300
        Width = 45
        Height = 13
        AutoSize = False
      end
      object Bevel1: TBevel
        Left = 12
        Top = 260
        Width = 465
        Height = 5
        Shape = bsTopLine
      end
      object ComboBoxDevice: TComboBox
        Left = 92
        Top = 267
        Width = 381
        Height = 22
        Style = csOwnerDrawFixed
        ItemHeight = 16
        TabOrder = 4
        OnChange = ComboBoxDeviceChange
      end
      object ButtonSource: TButton
        Left = 348
        Top = 52
        Width = 125
        Height = 25
        Caption = 'Источник сигнала...'
        Enabled = False
        TabOrder = 2
        OnClick = ButtonSourceClick
      end
      object ButtonFormat: TButton
        Left = 348
        Top = 80
        Width = 125
        Height = 25
        Caption = 'Формат кадра...'
        Enabled = False
        TabOrder = 3
        OnClick = ButtonFormatClick
      end
      object PanelScreen: TPanel
        Left = 11
        Top = 11
        Width = 322
        Height = 242
        BevelOuter = bvLowered
        Color = clBlack
        TabOrder = 0
      end
      object CheckBoxPreview: TCheckBox
        Left = 348
        Top = 16
        Width = 97
        Height = 17
        Caption = 'Просмотр'
        TabOrder = 1
        OnClick = CheckBoxPreviewClick
      end
      object TrackBarRate: TTrackBar
        Left = 196
        Top = 298
        Width = 221
        Height = 18
        Max = 24
        Min = 1
        Orientation = trHorizontal
        Frequency = 1
        Position = 3
        SelEnd = 0
        SelStart = 0
        TabOrder = 5
        ThumbLength = 14
        TickMarks = tmBoth
        TickStyle = tsNone
        OnChange = TrackBarRateChange
      end
    end
    object TabSheetCodec: TTabSheet
      Caption = 'Сжатие и файлы'
      ImageIndex = 1
      object Bevel6: TBevel
        Left = 4
        Top = 4
        Width = 481
        Height = 321
        Shape = bsFrame
      end
      object Bevel4: TBevel
        Left = 108
        Top = 16
        Width = 277
        Height = 17
      end
      object Label4: TLabel
        Left = 20
        Top = 56
        Width = 127
        Height = 13
        Alignment = taRightJustify
        Caption = 'Директория для файлов:'
      end
      object Bevel3: TBevel
        Left = 12
        Top = 44
        Width = 465
        Height = 5
        Shape = bsTopLine
      end
      object Label2: TLabel
        Left = 70
        Top = 18
        Width = 34
        Height = 13
        Alignment = taRightJustify
        Caption = 'Кодек:'
      end
      object LabelCodec: TLabel
        Left = 112
        Top = 18
        Width = 269
        Height = 13
        AutoSize = False
      end
      object Label6: TLabel
        Left = 22
        Top = 116
        Width = 124
        Height = 13
        Alignment = taRightJustify
        Caption = 'Начинать новый файл в:'
      end
      object Label7: TLabel
        Left = 28
        Top = 80
        Width = 118
        Height = 13
        Alignment = taRightJustify
        Caption = 'Размер хранимых (Gb):'
      end
      object Bevel5: TBevel
        Left = 12
        Top = 104
        Width = 465
        Height = 5
        Shape = bsTopLine
      end
      object Bevel7: TBevel
        Left = 12
        Top = 140
        Width = 465
        Height = 5
        Shape = bsTopLine
      end
      object Label3: TLabel
        Left = 200
        Top = 116
        Width = 20
        Height = 13
        Caption = 'час.'
      end
      object Label5: TLabel
        Left = 276
        Top = 116
        Width = 23
        Height = 13
        Caption = 'мин.'
      end
      object EditDir: TEdit
        Left = 152
        Top = 52
        Width = 297
        Height = 21
        TabOrder = 1
        OnExit = EditDirExit
      end
      object ButtonCompressor: TButton
        Left = 392
        Top = 12
        Width = 85
        Height = 25
        Caption = 'Выбрать...'
        TabOrder = 0
        OnClick = ButtonCompressorClick
      end
      object ButtonDir: TButton
        Left = 452
        Top = 52
        Width = 23
        Height = 21
        Caption = '...'
        Enabled = False
        TabOrder = 2
        OnClick = ButtonDirClick
      end
      object ComboBoxFileTimeH: TComboBox
        Left = 152
        Top = 112
        Width = 45
        Height = 22
        Style = csOwnerDrawFixed
        ItemHeight = 16
        TabOrder = 4
        OnExit = ComboBoxFileTimeHExit
      end
      object ComboBoxFileTimeM: TComboBox
        Left = 228
        Top = 112
        Width = 45
        Height = 22
        Style = csOwnerDrawFixed
        ItemHeight = 16
        TabOrder = 5
        OnExit = ComboBoxFileTimeHExit
      end
      object ComboBoxFilesSize: TComboBox
        Left = 152
        Top = 76
        Width = 77
        Height = 22
        Style = csOwnerDrawFixed
        ItemHeight = 16
        TabOrder = 3
        OnExit = ComboBoxFilesSizeExit
      end
    end
  end
end
