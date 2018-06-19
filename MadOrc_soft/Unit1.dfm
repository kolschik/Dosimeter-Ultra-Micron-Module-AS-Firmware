object Form1: TForm1
  Left = 0
  Top = 0
  Width = 1013
  Height = 507
  HorzScrollBar.Smooth = True
  HorzScrollBar.Tracking = True
  VertScrollBar.Smooth = True
  VertScrollBar.Tracking = True
  AutoScroll = True
  Caption = 'Stabilizer'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object JvChart1: TJvChart
    Left = 217
    Top = 8
    Width = 1935
    Height = 430
    Options.XAxisValuesPerDivision = 0
    Options.XAxisLabelAlignment = taLeftJustify
    Options.XAxisDateTimeMode = False
    Options.PenCount = 3
    Options.XOrigin = 0
    Options.YOrigin = 0
    Options.PrimaryYAxis.YMax = 10.000000000000000000
    Options.PrimaryYAxis.YLegendDecimalPlaces = 0
    Options.SecondaryYAxis.YMax = 10.000000000000000000
    Options.SecondaryYAxis.YLegendDecimalPlaces = 0
    Options.MouseDragObjects = False
    Options.LegendRowCount = 0
    Options.AxisLineWidth = 3
    Options.HeaderFont.Charset = DEFAULT_CHARSET
    Options.HeaderFont.Color = clWindowText
    Options.HeaderFont.Height = -11
    Options.HeaderFont.Name = 'Tahoma'
    Options.HeaderFont.Style = []
    Options.LegendFont.Charset = DEFAULT_CHARSET
    Options.LegendFont.Color = clWindowText
    Options.LegendFont.Height = -11
    Options.LegendFont.Name = 'Tahoma'
    Options.LegendFont.Style = []
    Options.AxisFont.Charset = DEFAULT_CHARSET
    Options.AxisFont.Color = clWindowText
    Options.AxisFont.Height = -11
    Options.AxisFont.Name = 'Tahoma'
    Options.AxisFont.Style = []
    Options.PaperColor = clWhite
    Options.AxisLineColor = clBlack
    Options.CursorColor = clBlack
    Options.CursorStyle = psSolid
  end
  object Label2: TLabel
    Left = 87
    Top = 8
    Width = 124
    Height = 18
    Caption = #1050#1072#1085#1072#1083' '#1082#1072#1083#1080#1073#1088#1086#1074#1082#1080
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 87
    Top = 38
    Width = 85
    Height = 18
    Caption = #1054#1082#1085#1086' '#1087#1086#1080#1089#1082#1072
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 87
    Top = 104
    Width = 110
    Height = 18
    Caption = #1056#1077#1087#1077#1088#1085#1099#1081' '#1082#1072#1085#1072#1083
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 87
    Top = 132
    Width = 88
    Height = 18
    Caption = #1056'-'#1082#1086#1101#1092#1080#1094#1077#1085#1090
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Channel: TEdit
    Left = 16
    Top = 11
    Width = 65
    Height = 21
    NumbersOnly = True
    TabOrder = 0
    Text = '0'
  end
  object Window: TEdit
    Left = 16
    Top = 39
    Width = 65
    Height = 21
    NumbersOnly = True
    TabOrder = 1
    Text = '0'
  end
  object Found: TEdit
    Left = 16
    Top = 105
    Width = 65
    Height = 21
    NumbersOnly = True
    ReadOnly = True
    TabOrder = 2
    Text = '0'
  end
  object Button1: TButton
    Left = 16
    Top = 66
    Width = 181
    Height = 25
    Caption = #1053#1072#1095#1072#1090#1100' '#1087#1086#1080#1089#1082' '#1088#1077#1087#1077#1088#1072
    TabOrder = 3
    OnClick = Button1Click
  end
  object Rep_coefficent: TEdit
    Left = 16
    Top = 132
    Width = 65
    Height = 21
    NumbersOnly = True
    ReadOnly = True
    TabOrder = 4
    Text = '0'
  end
  object Rep_save: TButton
    Left = 16
    Top = 159
    Width = 181
    Height = 25
    Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1088#1077#1087#1077#1088
    TabOrder = 5
    OnClick = Rep_saveClick
  end
  object Button2: TButton
    Left = 16
    Top = 190
    Width = 181
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1080#1090#1100' '#1089#1090#1072#1073#1080#1083#1080#1079#1072#1094#1080#1102
    TabOrder = 6
    OnClick = Button2Click
  end
  object dreif: TButton
    Left = 16
    Top = 221
    Width = 181
    Height = 25
    Caption = #1053#1072#1095#1072#1090#1100' '#1072#1085#1072#1083#1080#1079' '#1076#1088#1077#1081#1092#1072
    TabOrder = 7
    OnClick = dreifClick
  end
  object StringGrid1: TStringGrid
    Left = 40
    Top = 252
    Width = 145
    Height = 191
    ColCount = 1
    FixedCols = 0
    RowCount = 2047
    TabOrder = 8
    ColWidths = (
      113)
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 176
  end
  object Timer_dreif: TTimer
    Enabled = False
    Interval = 300000
    OnTimer = Timer_dreifTimer
    Left = 136
  end
  object SaveDialog1: TSaveDialog
    Left = 24
    Top = 8
  end
end
