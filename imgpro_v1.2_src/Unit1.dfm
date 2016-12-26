object Form1: TForm1
  Left = 648
  Top = 546
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'IMG Pro v1.2'
  ClientHeight = 414
  ClientWidth = 624
  Color = clActiveBorder
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonAdd: TButton
    Left = 536
    Top = 32
    Width = 73
    Height = 25
    Caption = 'Addd...'
    TabOrder = 0
    OnClick = ButtonAddClick
  end
  object ListView1: TListView
    Left = 0
    Top = 0
    Width = 521
    Height = 345
    Columns = <
      item
        Caption = 'File Name'
        Width = 128
      end
      item
        Caption = 'Size'
        Width = 60
      end
      item
        Caption = 'Type'
        Width = 40
      end
      item
        AutoSize = True
        Caption = 'Path'
      end>
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    MultiSelect = True
    ReadOnly = True
    ParentFont = False
    TabOrder = 1
    ViewStyle = vsReport
    OnColumnClick = ListView1ColumnClick
    OnCompare = ListView1Compare
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 352
    Width = 609
    Height = 52
    Caption = '*.IMG file'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    object EditImgPath: TEdit
      Left = 8
      Top = 18
      Width = 497
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
    end
    object ButtonBrowseImgFile: TButton
      Left = 520
      Top = 17
      Width = 75
      Height = 25
      Caption = 'Browse...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = ButtonBrowseImgFileClick
    end
  end
  object ButtonReplace: TButton
    Left = 536
    Top = 216
    Width = 75
    Height = 25
    Caption = 'Replace'
    Enabled = False
    TabOrder = 3
    OnClick = ButtonReplaceClick
  end
  object ButtonDelete: TButton
    Left = 536
    Top = 72
    Width = 75
    Height = 25
    Caption = 'Delete'
    TabOrder = 4
    OnClick = ButtonDeleteClick
  end
  object ButtonAbout: TButton
    Left = 613
    Top = 0
    Width = 11
    Height = 17
    Caption = '?'
    TabOrder = 5
    OnClick = ButtonAboutClick
  end
  object ButtonExtract: TButton
    Left = 536
    Top = 176
    Width = 75
    Height = 25
    Caption = 'Extract...'
    Enabled = False
    TabOrder = 6
    OnClick = ButtonExtractClick
  end
  object ProgressBar1: TProgressBar
    Left = 531
    Top = 256
    Width = 83
    Height = 9
    Min = 0
    Max = 1
    Step = 1
    TabOrder = 7
  end
  object OpenDialog1: TOpenDialog
    Filter = 
      'GTA Car Files(*.dff;*.txd;*.ifp)|*.dff;*.txd;*.ifp|GTA Textures(' +
      '*.txd)|*.txd|All GTA Files(*.dff;*.txd;*.col;*.ipl;*.ifp;*.scm;*' +
      '.cut;*.dat;*.rrr;*.anm)|*.dff;*.txd;*.col;*.ipl;*.ifp;*.scm;*.cu' +
      't;*.dat;*.rrr;*.anm|All Files(*.*)|*.*'
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofEnableSizing]
    Left = 576
    Top = 312
  end
  object OpenDialog2: TOpenDialog
    Filter = 'GTA IMG Arhive (*.img)|*.img'
    Left = 544
    Top = 312
  end
end
