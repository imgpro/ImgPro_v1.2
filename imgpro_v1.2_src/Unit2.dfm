object AboutBox: TAboutBox
  Left = 419
  Top = 776
  VertScrollBar.Style = ssHotTrack
  BorderStyle = bsToolWindow
  Caption = 'IMG Pro v1.2 - About'
  ClientHeight = 101
  ClientWidth = 181
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = True
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 60
    Top = 8
    Width = 58
    Height = 13
    Caption = #1040#1074#1090#1086#1088': LOD'
  end
  object Label2: TLabel
    Left = 28
    Top = 32
    Width = 32
    Height = 13
    Caption = 'E-Mail:'
  end
  object Label3: TLabel
    Left = 44
    Top = 64
    Width = 89
    Height = 25
    Alignment = taCenter
    AutoSize = False
    BiDiMode = bdLeftToRight
    Caption = 'Copyright (C) 2007 LOD'
    ParentBiDiMode = False
    WordWrap = True
  end
  object Edit1: TEdit
    Left = 64
    Top = 32
    Width = 97
    Height = 17
    BorderStyle = bsNone
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 0
    Text = 'imgpro@rambler.ru'
  end
end
