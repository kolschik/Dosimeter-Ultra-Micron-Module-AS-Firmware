unit main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, JvTrayIcon, JvComponentBase, ImgList, Registry,
  Menus, StdCtrls, XPMan, ExtCtrls, jpeg, JvExControls, JvPoweredBy,
  shellapi, JvExExtCtrls, MMSystem, iaRS232, Vcl.ExtDlgs, pngimage,
  ShlObj, IdAuthentication, IdBaseComponent, IdComponent, IdTCPConnection,
  IdTCPClient, IdHTTP, IdIOHandler, IdIOHandlerSocket, IdIOHandlerStack, IdSSL,
  IdSSLOpenSSL, IdMultipartFormData, System.DateUtils, About_f, JvChart;

type
  TForm1 = class(TForm)
    // procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  // Здесь необходимо описать класс TMyThread:
  // TMyThread = class(TThread)
  // private
  // { Private declarations }
  // protected
  // procedure Execute; override;
  // end;

  TmainFrm = class(TForm)
    MyTray: TJvTrayIcon;
    TrayMenu: TPopupMenu;
    ExitBtn: TMenuItem;
    AboutBtn: TMenuItem;
    XPManifest1: TXPManifest;
    Timer1: TTimer;
    PortSub: TMenuItem;
    COM11: TMenuItem;
    COM21: TMenuItem;
    COM31: TMenuItem;
    COM41: TMenuItem;
    COM51: TMenuItem;
    Label9: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    SaveDialog1: TSaveDialog;
    Com_detect: TTimer;
    Auto1: TMenuItem;
    Button4: TButton;
    Button5: TButton;
    ImageList1: TImageList;
    EVolt: TEdit;
    Label1: TLabel;
    Voltage_refresh: TButton;
    Label2: TLabel;
    Label4: TLabel;
    Counts: TEdit;
    Pump: TEdit;
    AKB_Volt: TEdit;
    Temperature: TEdit;
    Label15: TLabel;
    Panel1: TPanel;
    Label8: TLabel;
    Label17: TLabel;
    LED: TComboBox;
    Label18: TLabel;
    TCorr: TEdit;
    Panel2: TPanel;
    Sound: TEdit;
    Label5: TLabel;
    Label6: TLabel;
    Spectro_time: TEdit;
    Source: TComboBox;
    Label7: TLabel;
    Selected_time: TComboBox;
    Incative: TTimer;
    Label12: TLabel;
    Start_channel: TEdit;
    Label13: TLabel;
    ADC_time: TComboBox;
    Label14: TLabel;
    Chart: TJvChart;
    ScrollBox1: TScrollBox;
    Graph_plus: TButton;
    Graph_minus: TButton;
    Scale_zero: TButton;
    Allow_precis_stable: TCheckBox;
    Bits: TButton;
    Tot_cnt: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure ExitBtnClick(Sender: TObject);
    procedure OKBtnClick();
    procedure MyTrayDoubleClick(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure MyTrayClick(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure N1Click(Sender: TObject);
    procedure AboutBtnClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure COM11Click(Sender: TObject);
    procedure COM21Click(Sender: TObject);
    procedure COM31Click(Sender: TObject);
    procedure COM41Click(Sender: TObject);
    procedure COM51Click(Sender: TObject);
    procedure CloseTimerTimer(Sender: TObject);
    procedure Com_detectTimer(Sender: TObject);
    procedure Auto1Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Voltage_refreshClick(Sender: TObject);
    procedure EVoltChange(Sender: TObject);
    procedure SourceChange(Sender: TObject);
    procedure Selected_timeChange(Sender: TObject);
    procedure IncativeTimer(Sender: TObject);
    procedure Graph_plusClick(Sender: TObject);
    procedure Graph_minusClick(Sender: TObject);
    procedure Scale_zeroClick(Sender: TObject);
    procedure BitsClick(Sender: TObject);

  private
    fBuf: TiaBuf;
    fBufCount: Integer;
    procedure DoOnReceiveEvent(Sender: TObject; Const aData: TiaBuf;
      aCount: Cardinal);
    procedure DoOnSendedEvent(Sender: TObject; Const aData: TiaBuf;
      aCount: Cardinal);

    { Private declarations }
  protected
  public
    RS232: TiaRS232;
    procedure SaveReg;
    function GetMyVersion: uint;
    function PosR2L(const FindS, SrcS: string): Integer;
    procedure WMPowerBroadcast(var MyMessage: TMessage);
      message WM_POWERBROADCAST;
    procedure WMSysCommand(var Message: TMessage); message WM_SYSCOMMAND;
  end;

var
  mainFrm: TmainFrm;
  // MyThread: TMyThread;
  FeatureReportLen: Integer = 0;
  DevPresent: boolean = false;
  VoltChange: boolean = false;
  Timed_spectr: boolean = false;
  DenyCommunications: boolean = false;
  massive_11bit: boolean = true;
  pingback: Integer;
  time: Integer;
  needexit: boolean = false;
  tempcol: TColor = clLime;
  bmp: TBitmap;
  png: TPNGObject;
  IMPS: uint = 0;
  blinker: boolean;
  i: uchar;
  temperature: uchar;

  serial_active: boolean = false;
  comport_number_select: uint = 6;
  comport_number: uint = 1;
  USB_massive_loading: boolean = false;

  Need_save_csv: boolean = false;
  Need_load_flash: boolean = false;

  max_address: uint = 2047;

  spectra_massive: array [0 .. 2048] of UInt32;

  spectra_massive_1_sec: array [0 .. 2048] of UInt32;
  spectra_massive_2_sec: array [0 .. 2048] of UInt32;
  spectra_massive_3_sec: array [0 .. 2048] of UInt32;
  spectra_massive_4_sec: array [0 .. 2048] of UInt32;
  spectra_massive_5_sec: array [0 .. 2048] of UInt32;
  spectra_massive_6_sec: array [0 .. 2048] of UInt32;

  spectra_massive_ready: array [0 .. 2048] of boolean;

  pump_massive: array [0 .. 60] of UInt32;

  address: UInt32 = 0;
  address_last: UInt32 = 0;

  usb_send_try: UInt32 = 0;

  Graph_scale: UInt32 = 1;
  Total_counts: UInt32 = 0;
  Spectro_time_raw: UInt32 = 0;

  myDate: TDateTime;
  formattedDateTime: string;
  Voltage_level: Integer;

  firmware_date: string;

implementation

{$R *.dfm}

function TmainFrm.PosR2L(const FindS, SrcS: string): Integer;
{ Функция возвращает начало последнего вхождения
  подстроки FindS в строку SrcS, т.е. первое с конца.
  Если возвращает ноль, то подстрока не найдена.
  Можно использовать в текстовых редакторах
  при поиске текста вверх от курсора ввода. }

  function InvertS(const S: string): string;
  { Инверсия строки S }
  var
    i, Len: Integer;
  begin
    Len := Length(S);
    SetLength(Result, Len);
    for i := 1 to Len do
      Result[i] := S[Len - i + 1];
  end;

var
  ps: Integer;
begin
  { Например: нужно найти последнее вхождение
    строки 'ро' в строке 'пирожок в коробке'.
    Инвертируем обе строки и получаем
    'ор' и 'екборок в кожорип',
    а затем ищем первое вхождение с помощью стандартной
    функции Pos(Substr, S: string): string;
    Если подстрока Substr есть в строке S, то
    эта функция возвращает позицию первого вхождения,
    а иначе возвращает ноль. }
  ps := Pos(InvertS(FindS), InvertS(SrcS));
  { Если подстрока найдена определяем её истинное положение
    в строке, иначе возвращаем ноль }
  if ps <> 0 then
    Result := Length(SrcS) - Length(FindS) - ps + 2
  else
    Result := 0;
end;

/// //////////////////////////////////////////////////
function TmainFrm.GetMyVersion: uint;
type
  TVerInfo = packed record
    Nevazhno: array [0 .. 47] of byte; // ненужные нам 48 байт
    Minor, Major, Build, Release: word; // а тут версия
  end;
var
  S: TResourceStream;
  v: TVerInfo;
begin
  Result := 0;
  try
    S := TResourceStream.Create(HInstance, '#1', RT_VERSION); // достаём ресурс
    if S.Size > 0 then
    begin
      S.Read(v, SizeOf(v)); // читаем нужные нам байты
      Result := v.Build;
    end;
    S.Free;
  except
    ;
  end;
end;

// =============================================================================
procedure TmainFrm.OKBtnClick();
begin
  MyTray.HideApplication;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.WMSysCommand(var Message: TMessage);
begin
  if Message.WParam = SC_MAXIMIZE then
  begin
    Message.Result := 0;
  end
  else if Message.WParam = SC_MINIMIZE then
  begin
    OKBtnClick();
    Message.Result := 0;
  end
  else if Message.WParam = SC_CLOSE then
  begin
    OKBtnClick();
    Message.Result := 0;
  end
  else
    inherited;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.WMPowerBroadcast(var MyMessage: TMessage);
begin
  if MyMessage.Msg = WM_POWERBROADCAST then
  begin
    if MyMessage.WParam = PBT_APMRESUMEAUTOMATIC then
    begin
      DenyCommunications := false;
    end
    else
    begin
      DevPresent := false;
      firmware_date := '';

      DenyCommunications := true;
      RS232.Close;
    end;
  end;
  inherited;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.SaveReg;
var
  reg: TRegistry;
  dir: string;
  exe: string;
  f: TextFile;
begin
  reg := TRegistry.Create; // Открываем реестр
  reg.RootKey := HKEY_CURRENT_USER; // Для текущего пользователя
  reg.OpenKey('Software\Micron\Spectr', true); // Открываем раздел

  reg.WriteInteger('comport', comport_number_select);
  reg.CloseKey; // Закрываем раздел
  reg.Free;
end;
procedure TmainFrm.Scale_zeroClick(Sender: TObject);
begin

  Graph_scale:=1;

end;

// =============================================================================

// =============================================================================
procedure TmainFrm.Selected_timeChange(Sender: TObject);
begin
   time:=Integer(mainFrm.Selected_time.Items.Objects[mainFrm.Selected_time.ItemIndex]);
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.SourceChange(Sender: TObject);
begin

  if(mainFrm.Source.ItemIndex>0) then
  begin
    mainFrm.Selected_time.Enabled:=false;
    time:=0;
  end
  else
  begin
    mainFrm.Selected_time.Enabled:=true;
    mainFrm.Selected_time.Itemindex:=0;
    time:=0;
  end;


// clWindow
end;

// =============================================================================


// =============================================================================
procedure TmainFrm.FormCreate(Sender: TObject);
var
  reg: TRegistry;
  dir: string;
  exe: string;
  f: TextFile;
  vbar:TJvChartVerticalBar;
  hbar:TJvChartHorizontalBar;

begin

mainFrm.Chart.Options.XEnd:=2048;
mainFrm.Chart.Options.PenColor[0]:=clRed;
mainFrm.Chart.Options.PenStyle[0]:=psSolid;

mainFrm.Chart.Options.PenColor[1]:=clBlack;
mainFrm.Chart.Options.PenStyle[1]:=psSolid;
Chart.Options.GradientColor := $00FDEEDB; // powder blue (baby blue) mostly white.
Chart.Options.GradientDirection :=grDown;

//   Chart.Options.XAxisLegendSkipBy := 5;
//    Chart.Options.XAxisValuesPerDivision := 24;




mainFrm.LED.AddItem('10%', nil);
mainFrm.LED.AddItem('20%', nil);
mainFrm.LED.AddItem('30%', nil);
mainFrm.LED.AddItem('50%', nil);
mainFrm.LED.AddItem('70%', nil);
mainFrm.LED.AddItem('90%', nil);
mainFrm.LED.AddItem('100%', nil);

mainFrm.Source.AddItem('Он-лайн', nil);
mainFrm.Source.AddItem('Flash', nil);
mainFrm.Source.ItemIndex:=0;

mainFrm.ADC_time.AddItem('0.25', nil);
mainFrm.ADC_time.AddItem('0.56', nil);
mainFrm.ADC_time.AddItem('1.00', nil);
mainFrm.ADC_time.AddItem('1.50', nil);
mainFrm.ADC_time.ItemIndex:=0;

mainFrm.Panel2.Visible:=false;
mainFrm.Panel1.Visible:=false;

mainFrm.Selected_time.Items.AddObject('Без остановки', TObject(0));
mainFrm.Selected_time.Items.AddObject('1 минута',      TObject(60));
mainFrm.Selected_time.Items.AddObject('5 минут',       TObject(300));
mainFrm.Selected_time.Items.AddObject('10 минут',      TObject(600));
mainFrm.Selected_time.Items.AddObject('30 минут',      TObject(1800));
mainFrm.Selected_time.Items.AddObject('1 час',         TObject(3600));
mainFrm.Selected_time.Items.AddObject('2 часа',        TObject(7200));
mainFrm.Selected_time.Items.AddObject('3 часа',        TObject(10800));
mainFrm.Selected_time.Items.AddObject('5 часов',       TObject(18000));
mainFrm.Selected_time.Items.AddObject('6 часов',       TObject(21600));
mainFrm.Selected_time.Items.AddObject('10 часов',      TObject(36000));
mainFrm.Selected_time.Items.AddObject('15 часов',      TObject(54000));
mainFrm.Selected_time.Items.AddObject('24 часа',      TObject(86400));
mainFrm.Selected_time.ItemIndex:=0;


  mainFrm.Caption := 'Module-A* build:' + IntToStr(GetMyVersion);
//  Windows.EnableMenuItem(GetSystemMenu(Handle, false), SC_CLOSE,
//    MF_DISABLED or MF_GRAYED);
  GetSystemMenu(Handle, false);
  Perform(WM_NCPAINT, Handle, 0);

  RS232 := TiaRS232.Create;
  RS232.OnRSReceived := DoOnReceiveEvent;
  RS232.OnRSSended := DoOnSendedEvent;
  SetLength(fBuf, 64);
  fBufCount := 0;

  // Setting the default value

  MyTray.IconIndex := -1;
  reg := TRegistry.Create; // Открываем реестр
  reg.RootKey := HKEY_CURRENT_USER;
  if reg.OpenKey('Software\Micron\Spectr', false) then
  begin
    try
      comport_number_select := reg.ReadInteger('comport');
    except
      comport_number_select := 6;
    end;
  end
  else
  begin // если нет раздела -> прога еще не запускалась
    reg.OpenKey('Software\Micron\Spectr', true);
    try
      reg.WriteInteger('comport', 1);
    except
    end;
    reg.CloseKey; // Закрываем раздел
  end;
  reg.CloseKey; // Закрываем раздел
  reg.Free;

  if (comport_number_select = 1) then
    COM11.Checked := true;
  if (comport_number_select = 2) then
    COM21.Checked := true;
  if (comport_number_select = 3) then
    COM31.Checked := true;
  if (comport_number_select = 4) then
    COM41.Checked := true;
  if (comport_number_select = 5) then
    COM51.Checked := true;
  if (comport_number_select = 6) then
    Auto1.Checked := true;

end;
// =============================================================================

// =============================================================================
procedure TmainFrm.CloseTimerTimer(Sender: TObject);
begin
  if (RS232.Active = true) then
  begin
//    RS232.StopListner;
//    RS232.Close;
//    CloseTimer.Enabled := false;
//    CloseTimer.interval := 100;
  end;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.COM11Click(Sender: TObject);
begin
  comport_number_select := 1;
  SaveReg;
  COM11.Checked := true;
  COM21.Checked := false;
  COM31.Checked := false;
  COM41.Checked := false;
  COM51.Checked := false;
  Auto1.Checked := false;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.COM21Click(Sender: TObject);
begin
  comport_number_select := 2;
  SaveReg;
  COM11.Checked := false;
  COM21.Checked := true;
  COM31.Checked := false;
  COM41.Checked := false;
  COM51.Checked := false;
  Auto1.Checked := false;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.COM31Click(Sender: TObject);
begin
  comport_number_select := 3;
  SaveReg;
  COM11.Checked := false;
  COM21.Checked := false;
  COM31.Checked := true;
  COM41.Checked := false;
  COM51.Checked := false;
  Auto1.Checked := false;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.COM41Click(Sender: TObject);
begin
  comport_number_select := 4;
  SaveReg;
  COM11.Checked := false;
  COM21.Checked := false;
  COM31.Checked := false;
  COM41.Checked := true;
  COM51.Checked := false;
  Auto1.Checked := false;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.COM51Click(Sender: TObject);
begin
  comport_number_select := 5;
  SaveReg;
  COM11.Checked := false;
  COM21.Checked := false;
  COM31.Checked := false;
  COM41.Checked := false;
  COM51.Checked := true;
  Auto1.Checked := false;
end;
// =============================================================================

procedure TmainFrm.Com_detectTimer(Sender: TObject);
var
  reg: TRegistry;
  tstrSubKeys: TStringList;
  strParentKey: String;
  q: uint;
  ixi: uint;
begin
  if (Auto1.Checked = true) then
  begin
    reg := TRegistry.Create; // Открываем реестр
    tstrSubKeys := TStringList.Create;
    reg.RootKey := HKEY_LOCAL_MACHINE;
    if reg.OpenKey('HARDWARE\DEVICEMAP\SERIALCOMM', false) then
    begin
      reg.GetValueNames(tstrSubKeys);
      if tstrSubKeys.Count > 0 then
      begin
        q := tstrSubKeys.Count - 1;
        for ixi := 0 to q do
        begin
          if (Pos('USBSER', tstrSubKeys[ixi]) > 0) then
          begin
            comport_number :=
              StrToInt(Copy(reg.ReadString(tstrSubKeys[ixi]), 4, 3));
          end;
        end;
      end;
    end;
    reg.CloseKey;
    reg.Free;
    tstrSubKeys.Free;
  end
  else
  begin
    comport_number := comport_number_select;
  end;
end;

// =============================================================================

// =============================================================================
procedure TmainFrm.EVoltChange(Sender: TObject);
begin
  VoltChange := true;
  mainFrm.Panel1.Color := clYellow;
end;

procedure TmainFrm.ExitBtnClick(Sender: TObject);
begin
  if (DevPresent = true) then
  begin
    DevPresent := false;
    firmware_date := '';
  end;

    application.Terminate;

end;
// =============================================================================


// =============================================================================
procedure TmainFrm.Auto1Click(Sender: TObject);
begin
  comport_number_select := 6;
  SaveReg;
  COM11.Checked := false;
  COM21.Checked := false;
  COM31.Checked := false;
  COM41.Checked := false;
  COM51.Checked := false;
  Auto1.Checked := true;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.MyTrayClick(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
//  if Button <> mbRight then
//  begin
//    if (not DevPresent) then
//      MyTray.BalloonHint('12', '34', TBalloonType(3), 5000, true)
//    else
//  end;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.MyTrayDoubleClick(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  MyTray.ShowApplication;
  VoltChange := false;
  mainFrm.Panel1.Color := clMenu;
  mainFrm.Timer1.Enabled := true;

end;
// =============================================================================

// =============================================================================
procedure TmainFrm.N1Click(Sender: TObject);
begin
  MyTray.ShowApplication;
  VoltChange := false;
  mainFrm.Panel1.Color := clMenu;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.BitsClick(Sender: TObject);
var
  ixx: uint;
begin

  if(massive_11bit) then
  begin
    massive_11bit:= false;
    mainFrm.Bits.Caption:='10b';
  end
  else
  begin
    massive_11bit:= true;
    mainFrm.Bits.Caption:='11b';
  end;

  for ixx := 0 to max_address do
  begin
    spectra_massive_6_sec[ixx]:=0;
    spectra_massive_5_sec[ixx]:=0;
    spectra_massive_4_sec[ixx]:=0;
    spectra_massive_3_sec[ixx]:=0;
    spectra_massive_2_sec[ixx]:=0;
    spectra_massive_1_sec[ixx]:=0;
    mainFrm.Chart.Data.Value[1,ixx]:=0;
    mainFrm.Chart.Data.Value[0,ixx]:=0;
  end;


end;

procedure TmainFrm.Button4Click(Sender: TObject);
var
  ix: uint;
begin

  Need_save_csv:=true;
  if(mainFrm.Source.ItemIndex > 0) then
    Need_load_flash:=true;

   address_last := max_address;
   for ix := 0 to max_address do
   begin
     spectra_massive[ix] := 0;
     spectra_massive_ready[ix] := false;
   end;


  end;
// =============================================================================

// =============================================================================
procedure TmainFrm.Button5Click(Sender: TObject);
var
  vAns: TiaBuf;
  ix: uint;
  ibx: uint;
begin
  begin
    RS232.Open;
    RS232.StartListner;
    if (RS232.Active) then
    begin
      DevPresent := true;
      SetLength(vAns, 1);
      vAns[0] := $04; //
      RS232.Send(vAns);
    end;
  end;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.Timer1Timer(Sender: TObject);
var
  vAns: TiaBuf;
  bytes_to_send: uint;
  i: uint;
  ix: uint;
  min,max,ixx: Integer;
begin

  mainFrm.Pump.Color:=$009BFFB5;

  max:=pump_massive[1];
  min:=pump_massive[1];

  for ixx := 2 to 10 do
  begin

    if(pump_massive[ixx]>max) then
      max:=pump_massive[ixx];

    if(pump_massive[ixx]<min) then
      min:=pump_massive[ixx];

  end;

  if(max-min > 30) then
    mainFrm.Pump.Color:=$009B9BFF;


  if (DenyCommunications = false) then
  begin
    if (USB_massive_loading = false) then
    begin

      if (RS232.Active = false) then
      begin

        RS232.Close;
        FreeAndNil(RS232);

        RS232 := TiaRS232.Create;
        RS232.OnRSReceived := DoOnReceiveEvent;
        RS232.OnRSSended := DoOnSendedEvent;
        RS232.Properties.PortNum := comport_number;

        RS232.Open;
        RS232.StartListner;

        mainFrm.Incative.Enabled:=False;
        mainFrm.Incative.Enabled:=True;

      end;
      if (RS232.Active) then
      begin
        DevPresent := true;

        bytes_to_send := 0;
        i := 0;

        if(Need_load_flash) then
        begin
          Need_load_flash := false;

          bytes_to_send := bytes_to_send + 4; // + запрос основных данных

          SetLength(vAns, bytes_to_send);

          vAns[0] := $39; // выполнить сброс счетчиков дозиметра
          vAns[1] := $40; // считать массив спектра их флеша
          vAns[2] := $05; // загрузить данные земера
          vAns[3] := $02; // считать массив спектра
          mainFrm.RS232.Send(vAns);
        end
        else
        begin
          bytes_to_send := bytes_to_send + 3; // + запрос основных данных

          SetLength(vAns, bytes_to_send);

          vAns[0] := $05;
          vAns[1] := $39; // выполнить сброс счетчиков дозиметра
          vAns[2] := $02; // считать массив спектра

        end;


        if Length(vAns) > 0 then
          RS232.Send(vAns);

      end
      else
      begin
        RS232.Close;

        FreeAndNil(RS232);

        RS232 := TiaRS232.Create;
        RS232.OnRSReceived := DoOnReceiveEvent;
        RS232.OnRSSended := DoOnSendedEvent;

      end;

    end;
  end;
end;
// =============================================================================


procedure TmainFrm.Voltage_refreshClick(Sender: TObject);
var
  vAns: TiaBuf;
  ix: uint;
begin
  RS232.Open;
  RS232.StartListner;
  if (RS232.Active) then
  begin
    DevPresent := true;

    VoltChange := false;
    mainFrm.Panel1.Color := clMenu;

    ix := StrToInt(mainFrm.EVolt.Text);

    SetLength(vAns, 11);
    vAns[0] := $07; // Передача напряжения и настроек
    vAns[1] := ix and $FF;
    vAns[2] := (ix shr 8) and $FF;
    vAns[3] := (ix shr 16) and $FF;

    vAns[4] := StrToInt(mainFrm.Start_channel.Text) and $FF;
    vAns[5] := StrToInt(mainFrm.Sound.Text) and $FF;
    vAns[6] := mainFrm.LED.ItemIndex and $FF;
    vAns[7] := StrToInt(mainFrm.TCorr.Text) and $FF;
    vAns[8] := $00;
    vAns[9] := mainFrm.Allow_precis_stable.Checked.ToInteger and $FF;
    vAns[10] := mainFrm.ADC_time.ItemIndex and $FF;


    RS232.Send(vAns);

  end;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.AboutBtnClick(Sender: TObject);
begin
  try
    About.ShowModal;
  except
  end;
end;
// =============================================================================

// =============================================================================
procedure TmainFrm.FormDestroy(Sender: TObject);
begin
  FreeAndNil(RS232);
end;
// =============================================================================

// ================================================================================================================
procedure TmainFrm.DoOnReceiveEvent(Sender: TObject; const aData: TiaBuf;
  aCount: Cardinal);
Var
  ia: uint;
  f: Integer;
  cal_pointer: uint;
  eeprom_pointer: uint;
  ss: String;
  vCount: Integer;
  vBufCount: Integer;
  massive_element: UInt32;
  vAns: TiaBuf;
  ix: UInt32;
  iy: UInt32;
  buffer_len: UInt32;
  used_len: UInt32;
  packet_size: UInt32;
  aData_massive_pointer: UInt32;
  fBuf_pointer: UInt32;
  AIdHTTP: TIdHTTP;
  reg: TRegistry;
  key: String;
  data: TIdMultiPartFormDataStream;
  Fx: TextFile;
  FileName: string;
  i: Integer;
  used_bytes: Integer;
  tmp,ixx: Integer;
  TempStr: string;
  Y, M, D: word;
  voltage : Extended;
  cps : Extended;

begin
  packet_size := 64;
  aData_massive_pointer := 0;
  fBuf_pointer := 0;
  used_bytes := 0;

  mainFrm.Incative.Enabled:=False;
    mainFrm.Incative.Enabled:=True;

  For f := aData_massive_pointer to packet_size - 1 do
  Begin
      fBuf[f] := 0;
  end;


  While used_bytes < (Length(aData)-1) do
  begin
      mainFrm.Incative.Enabled:=False;
      mainFrm.Incative.Interval:=2000;
      mainFrm.Incative.Enabled:=True;
      // -----------------------------------------------------------------------------------
      if (aData[used_bytes] = $06) then
      begin // загрузка текущих данных

      mainFrm.Panel2.Visible:=true;
      mainFrm.Panel1.Visible:=true;

        mainFrm.Temperature.Text := IntToStr(aData[used_bytes + 1]);
        if (VoltChange = false) then
        begin
          mainFrm.EVolt.Text :=  IntToStr(aData[used_bytes + 2]  + (aData[used_bytes + 3]  shl 8)  + (aData[used_bytes + 4] shl 16));

          voltage:=aData[used_bytes + 5]  + (aData[used_bytes + 6]  shl 8);
          mainFrm.AKB_Volt.Text := FloatToStr(voltage/100);

          Spectro_time_raw:=  aData[used_bytes + 11] + (aData[used_bytes + 12] shl 8) + (aData[used_bytes + 13] shl 16) + (aData[used_bytes + 14] shl 24);
          mainFrm.Spectro_time.Text :=  IntToStr(Spectro_time_raw div 3600)+'ч '+IntToStr((Spectro_time_raw Mod 3600) div 60)+'м '+IntToStr(Spectro_time_raw Mod 60)+'с ('+IntToStr(Spectro_time_raw)+')';

          if(Spectro_time_raw>0) then
            cps:=Total_counts / Spectro_time_raw;

          mainFrm.Counts.Text := 'Now:' + IntToStr(aData[used_bytes + 7]  + (aData[used_bytes + 8]  shl 8) + (aData[used_bytes + 9] shl 16)  + (aData[used_bytes + 10] shl 24)) + 'cps  Avg:' + FloatToStrF(cps, ffFixed, 10, 2)+'cps  Tot:'+IntToStr(Total_counts div 1000)+'k';

          pump_massive[0]:=aData[used_bytes + 15] + (aData[used_bytes + 16] shl 8) + (aData[used_bytes + 17] shl 16) + (aData[used_bytes + 18] shl 24);
          mainFrm.Pump.Text :=     IntToStr(pump_massive[0]);

          for ixx := 0 to 59 do
          begin
            pump_massive[59-ixx]:=pump_massive[59-ixx-1];
          end;


          mainFrm.Start_channel.Text:= IntToStr(aData[used_bytes + 19]);
          mainFrm.Sound.Text :=        IntToStr(aData[used_bytes + 20]);
          mainFrm.LED.ItemIndex :=              aData[used_bytes + 21];
          mainFrm.TCorr.Text :=        IntToStr(aData[used_bytes + 22]);
          mainFrm.ADC_time.ItemIndex :=         aData[used_bytes + 23];
          mainFrm.Allow_precis_stable.Checked := aData[used_bytes + 24].ToBoolean;

          if ( mainFrm.Selected_time.Enabled = true ) then
          begin
            if ( time  > 0) then
              begin
                if ( time <= Spectro_time_raw ) then
                begin
                  mainFrm.Selected_time.ItemIndex:=0;
                  Timed_spectr:=true;
                  Need_save_csv:=true;
                  time:=0;
                end;
            end;
          end;
         end;


        used_bytes := used_bytes + 26;


      end else
      // -----------------------------------------------------------------------------------
      if (aData[used_bytes] = $03) then
      begin // загрузка элемента массива спектра

        address := aData[used_bytes + 1] shl 8;
        address := address + aData[used_bytes + 2];

        massive_element := aData[used_bytes + 3] shl 24;
        massive_element := massive_element + (aData[used_bytes + 4] shl 16);
        massive_element := massive_element + (aData[used_bytes + 5] shl 8);
        massive_element := massive_element + aData[used_bytes + 6];
        used_bytes := used_bytes + 7;

        if (address < max_address - 1) then
        begin
          spectra_massive[address] := massive_element;
          spectra_massive_ready[address] := true;

        end
        else
        begin

          if(massive_11bit = false) then
          begin
            for ixx := 0 to (max_address div 2) do
            begin
              tmp:=(spectra_massive[ixx*2]+spectra_massive[(ixx*2)+1]) div 2;
              spectra_massive[ixx*2]:=  tmp;
              spectra_massive[(ixx*2)+1]:=tmp;
            end;
          end;



          mainFrm.Chart.Options.PrimaryYAxis.YMax:=20;
          Total_counts:=0;
          for ixx := 0 to max_address do
          begin

            Total_counts:=Total_counts+spectra_massive[ixx];
            if((spectra_massive[ixx])>mainFrm.Chart.Options.PrimaryYAxis.YMax) then
                mainFrm.Chart.Options.PrimaryYAxis.YMax:=spectra_massive[ixx];

            if(spectra_massive[ixx] > mainFrm.Chart.Data.Value[0,ixx]) then
            begin
              spectra_massive_1_sec[ixx]:= Round(spectra_massive[ixx] - mainFrm.Chart.Data.Value[0,ixx]);
              mainFrm.Chart.Data.Value[0,ixx]:=spectra_massive[ixx];
            end
            else
            begin
              mainFrm.Chart.Data.Value[0,ixx]:=spectra_massive[ixx];
              spectra_massive_1_sec[ixx]:=0;
            end;
          end;


          if(Need_save_csv) then
          begin
            Timer1.Enabled := false;
            Need_save_csv:=false;
            SetLength(vAns, 1);
            vAns[0] := $39;

            USB_massive_loading := false;
            SaveDialog1.DefaultExt := '.csv';
            SaveDialog1.FileName := 'Spectr_'+mainFrm.Spectro_time.Text+'.csv';

// ============================================================================
            Timer1.Enabled := false;
            If Timed_spectr then
            begin
              Timed_spectr:=false;
              ShowMessage('Сбор спектра завершен!');
            end;

            If SaveDialog1.Execute then
            begin
              try
                AssignFile(Fx, SaveDialog1.FileName); // связали файл с переменной
                Rewrite(Fx); // создаем пустой файл
                for ixx := 0 to max_address do
                begin
                  WriteLn(Fx, IntToStr(ixx), ',', spectra_massive[ixx]);
                end;
              finally
                CloseFile(Fx);
              end;
            end;
            Timer1.Enabled := true;
// ============================================================================
          end;

          mainFrm.Chart.Options.PrimaryYAxis.YMax:=mainFrm.Chart.Options.PrimaryYAxis.YMax/Graph_scale;

          for ixx := 0 to max_address do
          begin
            mainFrm.Chart.Data.Value[1,ixx]:=(spectra_massive_1_sec[ixx] + spectra_massive_2_sec[ixx] + spectra_massive_3_sec[ixx] + spectra_massive_4_sec[ixx] + spectra_massive_5_sec[ixx] + spectra_massive_6_sec[ixx]) * (mainFrm.Chart.Options.PrimaryYAxis.YMax / 100);
            spectra_massive_6_sec[ixx]:=spectra_massive_5_sec[ixx];
            spectra_massive_5_sec[ixx]:=spectra_massive_4_sec[ixx];
            spectra_massive_4_sec[ixx]:=spectra_massive_3_sec[ixx];
            spectra_massive_3_sec[ixx]:=spectra_massive_2_sec[ixx];
            spectra_massive_2_sec[ixx]:=spectra_massive_1_sec[ixx];
          end;

          mainFrm.Chart.PlotGraph;

        end;
      end else Break;
      // -----------------------------------------------------------------------------------

//    end;
  end;
  if Length(vAns) > 0 then
    RS232.Send(vAns);
end;
// ================================================================================================================

// =============================================================================
procedure TmainFrm.DoOnSendedEvent(Sender: TObject; const aData: TiaBuf;
  aCount: Cardinal);
//Var
//  f: Integer;
//  ss: String;
begin

//  RS232.StopListner;
//  RS232.Close;
//  RS232.StopListner;


//  ss := '';
//  For f := 0 To Length(aData) - 1 do
//    ss := ss + IntToHex(aData[f], 2) + ' ';

end;

procedure TmainFrm.IncativeTimer(Sender: TObject);
begin
        RS232.Close;
        FreeAndNil(RS232);

        mainFrm.Incative.Enabled:=False;

        mainFrm.Panel2.Visible:=false;
        mainFrm.Panel1.Visible:=false;

        RS232 := TiaRS232.Create;
        RS232.OnRSReceived := DoOnReceiveEvent;
        RS232.OnRSSended := DoOnSendedEvent;
        RS232.Properties.PortNum := comport_number;

end;

procedure TmainFrm.Graph_minusClick(Sender: TObject);
begin

  if(Graph_scale > 1) then
  Graph_scale:=Graph_scale-1;

end;

procedure TmainFrm.Graph_plusClick(Sender: TObject);
begin

  if(Graph_scale < 10) then
  Graph_scale:=Graph_scale+1;

end;

end.
