unit main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, JvTrayIcon, JvComponentBase, ImgList, Registry,
  Menus, StdCtrls, XPMan, ExtCtrls, jpeg, JvExControls, JvPoweredBy,
  shellapi, JvExExtCtrls, MMSystem, iaRS232, Vcl.ExtDlgs, pngimage,
  ShlObj, IdAuthentication, IdBaseComponent, IdComponent, IdTCPConnection,
  IdTCPClient, IdHTTP, IdIOHandler, IdIOHandlerSocket, IdIOHandlerStack, IdSSL,
  IdSSLOpenSSL, IdMultipartFormData, System.DateUtils, About_f;

type
  TForm1 = class(TForm)
//    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

//Здесь необходимо описать класс TMyThread:
//  TMyThread = class(TThread)
//    private
//    { Private declarations }
//  protected
//    procedure Execute; override;
//  end;

  TmainFrm = class(TForm)
    MyTray: TJvTrayIcon;
    TrayMenu: TPopupMenu;
    ExitBtn: TMenuItem;
    AboutBtn: TMenuItem;
    XPManifest1: TXPManifest;
    MessTmr: TTimer;
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
    Timer3: TTimer;
    SaveDialog1: TSaveDialog;
    CloseTimer: TTimer;
    Com_detect: TTimer;
    Auto1: TMenuItem;
    Button4: TButton;
    Button5: TButton;
    ImageList1: TImageList;
    EVolt: TEdit;
    Label1: TLabel;
    Voltage_refresh: TButton;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure ExitBtnClick(Sender: TObject);
    procedure OKBtnClick();
    procedure MessTmrTimer(Sender: TObject);
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
    procedure Timer3Timer(Sender: TObject);
    procedure CloseTimerTimer(Sender: TObject);
    procedure Com_detectTimer(Sender: TObject);
    procedure Auto1Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Voltage_refreshClick(Sender: TObject);

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
    procedure RefreshRAD;
    procedure SaveReg;
    function GetMyVersion:uint;
    function PosR2L(const FindS, SrcS: string): Integer;
    procedure MakeIcon();
    procedure WMPowerBroadcast(var MyMessage: TMessage); message WM_POWERBROADCAST;
    procedure WMSysCommand(var Message:TMessage);        message WM_SYSCOMMAND;
end;

var
  mainFrm: TmainFrm;
//  MyThread: TMyThread;
  FeatureReportLen: integer = 0;
  DevPresent: boolean = false;
  DenyCommunications: boolean = false;
  pingback: integer;
  needexit: boolean = false;
  tempcol: TColor = clLime;
  bmp:TBitmap;
  png: TPNGObject;
  IMPS: uint = 0;
  blinker: boolean;
  i: uchar;
  temperature: uchar;

  serial_active:  boolean = false;
  comport_number_select: uint = 6;
  comport_number: uint = 1;
  USB_massive_loading: boolean = false;

  max_address: uint = 2047;

  spectra_massive: array[0..2048] of UInt32;

  spectra_massive_ready: array[0..2048] of boolean;

  address: UInt32 = 0;
  address_last: UInt32 = 0;

  usb_send_try: UInt32 = 0;

  myDate : TDateTime;
  formattedDateTime : string;
  Voltage_level: Integer;

  firmware_date: string;


implementation

{$R *.dfm}
{$R sounds.res}
uses Unit1;


function TmainFrm.PosR2L(const FindS, SrcS: string): Integer;
{Функция возвращает начало последнего вхождения
 подстроки FindS в строку SrcS, т.е. первое с конца.
 Если возвращает ноль, то подстрока не найдена.
 Можно использовать в текстовых редакторах
 при поиске текста вверх от курсора ввода.}

  function InvertS(const S: string): string;
    {Инверсия строки S}
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
  {Например: нужно найти последнее вхождение
   строки 'ро' в строке 'пирожок в коробке'.
   Инвертируем обе строки и получаем
     'ор' и 'екборок в кожорип',
   а затем ищем первое вхождение с помощью стандартной
   функции Pos(Substr, S: string): string;
   Если подстрока Substr есть в строке S, то
   эта функция возвращает позицию первого вхождения,
   а иначе возвращает ноль.}
  ps := Pos(InvertS(FindS), InvertS(SrcS));
  {Если подстрока найдена определяем её истинное положение
   в строке, иначе возвращаем ноль}
  if ps <> 0 then
    Result := Length(SrcS) - Length(FindS) - ps + 2
  else
    Result := 0;
end;

/////////////////////////////////////////////////////
function TmainFrm.GetMyVersion:uint;
type
  TVerInfo=packed record
    Nevazhno: array[0..47] of byte; // ненужные нам 48 байт
    Minor,Major,Build,Release: word; // а тут версия
  end;
var
  s:TResourceStream;
  v:TVerInfo;
begin
  result:=0;
  try
    s:=TResourceStream.Create(HInstance,'#1',RT_VERSION); // достаём ресурс
    if s.Size>0 then begin
      s.Read(v,SizeOf(v)); // читаем нужные нам байты
      result:=v.Build;
    end;
  s.Free;
  except; end;
end;


// =============================================================================
procedure TmainFrm.WMSysCommand(var Message: TMessage);
begin
    if Message.WParam = SC_MAXIMIZE then begin
        Message.Result := 0;
    end else if Message.WParam = SC_MINIMIZE then begin
        OKBtnClick();
        Message.Result := 0;
    end else if Message.WParam = SC_CLOSE then begin
        OKBtnClick();
        Message.Result := 0;
    end else inherited;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.WMPowerBroadcast(var MyMessage: TMessage);
begin
if MyMessage.Msg = WM_POWERBROADCAST then begin
if MyMessage.WParam = PBT_APMRESUMEAUTOMATIC then begin
    DenyCommunications:= false;
end
else begin
    DevPresent:= false;
    firmware_date:='';

    DenyCommunications:= true;
    RS232.StopListner;
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
  dir:string;
  exe:string;
  f:TextFile;
begin
  reg := TRegistry.Create;                               // Открываем реестр
  reg.RootKey := HKEY_CURRENT_USER;                      // Для текущего пользователя
  reg.OpenKey('Software\Micron\Spectr', true); // Открываем раздел

  reg.WriteInteger('comport',comport_number_select);
  reg.CloseKey;                                          // Закрываем раздел
  reg.Free;
end;
// =============================================================================



// =============================================================================
procedure TmainFrm.MakeIcon();
var
 ii: uint;
begin

  if (not DevPresent) then
    begin
  ImageList1.GetIcon(0,MyTray.Icon);
    end;
end;
// =============================================================================



// =============================================================================
procedure TmainFrm.RefreshRAD;
begin
    MakeIcon();
end;
// =============================================================================



// =============================================================================
procedure TmainFrm.FormCreate(Sender: TObject);
var
  reg: TRegistry;
  dir:string;
  exe:string;
  f:textfile;

begin

mainFrm.Caption:='Micron build:'+IntToStr(GetMyVersion);
   Windows.EnableMenuItem( GetSystemMenu( Handle, false ), SC_CLOSE, MF_DISABLED or MF_GRAYED );
   GetSystemMenu( Handle, false );
   Perform( WM_NCPAINT, Handle, 0 );

  RS232 := TiaRS232.Create;
  RS232.OnRSReceived := DoOnReceiveEvent;
  RS232.OnRSSended := DoOnSendedEvent;
  SetLength(fBuf, 64);
  fBufCount := 0;

  //Setting the default value

  MyTray.IconIndex := -1;
  MessTmr.Enabled := true;
  reg := TRegistry.Create;                              // Открываем реестр
  reg.RootKey := HKEY_CURRENT_USER;
  if reg.OpenKey('Software\Micron\Spectr', false) then
  begin
    try
      comport_number_select := reg.ReadInteger('comport');
    except
      comport_number_select := 6;
    end;
  end else begin    // если нет раздела -> прога еще не запускалась
    reg.OpenKey('Software\Micron\Spectr', true);
    try
      reg.WriteInteger('comport', 1);
    except
    end;
    reg.CloseKey;                                          // Закрываем раздел
  end;
  reg.CloseKey;                                        // Закрываем раздел
  reg.Free;

  if(comport_number_select=1)then COM11.Checked := true;
  if(comport_number_select=2)then COM21.Checked := true;
  if(comport_number_select=3)then COM31.Checked := true;
  if(comport_number_select=4)then COM41.Checked := true;
  if(comport_number_select=5)then COM51.Checked := true;
  if(comport_number_select=6)then Auto1.Checked := true;

end;
// =============================================================================


// =============================================================================
procedure TmainFrm.CloseTimerTimer(Sender: TObject);
begin
  if (RS232.Active = true) then
    begin
      RS232.StopListner;
      RS232.Close;
      CloseTimer.Enabled:=false;
      CloseTimer.interval:=100;
    end;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.COM11Click(Sender: TObject);
begin
comport_number_select:=1;
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
comport_number_select:=2;
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
comport_number_select:=3;
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
comport_number_select:=4;
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
comport_number_select:=5;
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
reg : TRegistry;
tstrSubKeys : TStringList;
strParentKey: String;
q:uint;
ixi:uint;
begin
if (Auto1.Checked = true) then
begin
  reg := TRegistry.Create;                              // Открываем реестр
  tstrSubKeys:= TStringList.Create;
  reg.RootKey := HKEY_LOCAL_MACHINE;
  if reg.OpenKey('HARDWARE\DEVICEMAP\SERIALCOMM', False) then begin
    reg.GetValueNames(tstrSubKeys);
    if tstrSubKeys.Count > 0 then
    begin
      q:=tstrSubKeys.Count-1;
      for ixi := 0 to q do
      begin
        if (Pos('USBSER',tstrSubKeys[ixi])>0) then
        begin
          comport_number:= StrToInt(
            Copy(reg.ReadString(
                                tstrSubKeys[ixi]
                                ), 4, 3));
        end;
      end;
    end;
  end;
  reg.CloseKey;
  reg.Free;
  tstrSubKeys.Free;
end else begin
comport_number:=comport_number_select;
end;
end;

// =============================================================================


// =============================================================================
procedure TmainFrm.ExitBtnClick(Sender: TObject);
begin
  if(DevPresent=true) then
   begin
      DevPresent:=false;
      firmware_date:='';
   end;

  needexit := true; // вежливо просим свалить из памяти
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.OKBtnClick();
begin
  MyTray.HideApplication;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.Auto1Click(Sender: TObject);
begin
comport_number_select:=6;
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
procedure TmainFrm.MessTmrTimer(Sender: TObject);
var
iz: uchar;
begin
  RefreshRAD;

    if needexit then
    application.Terminate;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.MyTrayClick(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
if Button <> mbRight then
begin
  if (not DevPresent) then
      MyTray.BalloonHint('12','34',TBalloonType(3),5000,true)
    else
end;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.MyTrayDoubleClick(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  MyTray.ShowApplication;
end;
// =============================================================================


// =============================================================================
procedure TmainFrm.N1Click(Sender: TObject);
begin
  MyTray.ShowApplication;
end;
// =============================================================================




// =============================================================================
procedure TmainFrm.Button4Click(Sender: TObject);
var
  vAns: TiaBuf;
  ix: uint;
  ibx: uint;
begin

Unit1.Form1.Show;
myDate:=Now;

Unit1.Form1.max_fon.Caption:='0%';
Unit1.Form1.impulses.Caption:='0%';
Unit1.Form1.errors.Caption:='0';

for ibx := 0 to max_address do
begin
  spectra_massive[ibx]:=0;
  spectra_massive_ready[ibx]:=false;
end;


Timer3.Enabled:=false;
Timer3.Interval:=3000;
Timer3.Enabled:=true;
address_last:=max_address;

begin
//  DevPresent:=false;

  RS232.Open;
  RS232.StartListner;
  CloseTimer.Enabled:=false;
  CloseTimer.Interval:=1500;
  CloseTimer.Enabled:=true;

  for ix := 0 to max_address do begin
    spectra_massive[ibx]:=0;
    spectra_massive_ready[ibx]:=false;
  end;

  if (RS232.Active)then
  begin
   DevPresent:=true;

   SetLength(vAns, 2);
   vAns[0]:=$39; // выполнить сброс счетчиков дозиметра
   vAns[1]:=$02; // считать массив спектра
   RS232.Send(vAns);

   USB_massive_loading:=true;

  end
  else
    Unit1.Form1.Close;
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
  CloseTimer.Enabled:=false;
  CloseTimer.Interval:=1500;
  CloseTimer.Enabled:=true;
  if (RS232.Active)then
  begin
   DevPresent:=true;
   SetLength(vAns, 1);
   vAns[0]:=$04; //
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
begin
if(DenyCommunications = false) then begin
if(USB_massive_loading = false) then begin

 if (RS232.Active = false)then
 begin
    RS232.Properties.PortNum  := comport_number;
    RS232.Open;
    RS232.StartListner;
    CloseTimer.Enabled:=false;
    CloseTimer.Interval:=100;
    CloseTimer.Enabled:=true;
 end;
    if (RS232.Active)then
    begin
      DevPresent:=true;

      bytes_to_send:=0;
      i:=0;

//      if (date_sent_flag=false) then
//        bytes_to_send:=bytes_to_send+7;

      bytes_to_send:=bytes_to_send+1; // + запрос основных данных

      SetLength(vAns, bytes_to_send);

      vAns[i]:=$05; i:=i+1; // запрос основных данных

//      if(firmware_date='') then
//      begin
//        vAns[i]:=$e5; i:=i+1; // считать дату прошивки
//        vAns[i]:=$e6; i:=i+1; // считать дату прошивки
//      end;


      if Length(vAns) > 0 then RS232.Send(vAns);

    end else begin
      if(DevPresent=true) then
      begin
        DevPresent:=false;
        firmware_date:='';
      end;
    end;



  end;
end;
end;
// =============================================================================




// =============================================================================
procedure TmainFrm.Timer3Timer(Sender: TObject);
var
  vAns: TiaBuf;
  iy: uint32;
begin

if(usb_send_try < 10) then
 begin
   if (address = address_last) and (USB_massive_loading = true) then
   begin
    SetLength(vAns, 1);
    usb_send_try:=usb_send_try+1;
    if Length(vAns) > 0 then RS232.Send(vAns);
   end
   else address_last:=address;
  end
  else
  begin
    address:=0;
    address_last:=0;
    usb_send_try:=0;
    RS232.StopListner;
    RS232.Close;
    DevPresent:= false;
    firmware_date:='';

    Unit1.Form1.Close;
    USB_massive_loading:=false;

  end;
end;
procedure TmainFrm.Voltage_refreshClick(Sender: TObject);
var
  vAns: TiaBuf;
  ix: uint;
begin
begin
  RS232.Open;
  RS232.StartListner;
  CloseTimer.Enabled:=false;
  CloseTimer.Interval:=1500;
  CloseTimer.Enabled:=true;
  if (RS232.Active)then
  begin
   DevPresent:=true;

   ix:=StrToInt(mainFrm.EVolt.Text);

   SetLength(vAns, 4);
   vAns[0]:=$07; // Передача напряжения
   vAns[1]:=ix and $ff;
   vAns[2]:=(ix shr 8) and $ff;
   vAns[3]:=(ix shr 16) and $ff;

   RS232.Send(vAns);
  end;
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
  RS232.StopListner;
  FreeAndNil(RS232);
end;
// =============================================================================




//================================================================================================================
procedure TmainFrm.DoOnReceiveEvent(Sender: TObject; const aData: TiaBuf; aCount: Cardinal);
Var
  ia:uint;
  F: Integer;
  cal_pointer: uint;
  eeprom_pointer: uint;
  ss: String;
  vCount: Integer;
  vBufCount: Integer;
  massive_element: UInt32 ;
  vAns: TiaBuf;
  ix : uint32;
  iy : uint32;
  buffer_len: uint32;
  used_len: uint32;
  packet_size: uint32;
  aData_massive_pointer: uint32;
  fBuf_pointer: uint32;
  AIdHTTP: TIdHTTP;
  reg: TRegistry;
  key: String;
  data: TIdMultiPartFormDataStream;
  Fx: TextFile;
  FileName: string;
  I: Integer;
  ixx: Integer;
  TempStr: string;
  Y, M, D: Word;


begin
packet_size:=7;
aData_massive_pointer:=0;
fBuf_pointer:=0;

CloseTimer.Enabled:=false;
CloseTimer.Interval:=1000;
CloseTimer.Enabled:=true;

While used_len<(Length(aData)-1) do begin
  // Заполнение массива пакета
   For F := aData_massive_pointer to aData_massive_pointer + packet_size - 1 do
     Begin
       fBuf[fBuf_pointer] := aData[F];
       Inc(fBuf_pointer);
       if F > (Length(aData)-1) then Break;
       used_len:=F;
     end;
   aData_massive_pointer:=used_len+1;
   fBuf_pointer:=0;

     begin
       ss := '';
       For F := 0 To packet_size - 1 do
         ss := ss + IntToHex(fBuf[F],2) + ' ';

//-----------------------------------------------------------------------------------
//if (fBuf[0] = $f7) then begin // чтение данных из EEPROM
//  eeprom_pointer := ((fBuf[1] shl 8)+fBuf[2]) div 4;
//  EEPROM_DATA[eeprom_pointer] := (fBuf[3] shl 24)+(fBuf[4] shl 16)+(fBuf[5] shl 8)+fBuf[6];
//  // Заполнение комбобокса изотопа
//  if eeprom_pointer = ($80 div 4) then
//    About_f.About.Isotop.ItemIndex:=Main.EEPROM_data[eeprom_pointer];
//end;


//-----------------------------------------------------------------------------------
//if (fBuf[0] = $e5) then begin // Дата прошивки
//  firmware_date:=Char(Ord(fBuf[1]))+
//                 Char(Ord(fBuf[2]))+
//                 ' '+
//                 Char(Ord(fBuf[3]))+
//                 Char(Ord(fBuf[4]))+
//                 Char(Ord(fBuf[5]))+
//                 ' 201'+
//                 Char(Ord(fBuf[6]));
//  HW_Label.Caption:='Ultra-Micron HW:'+hardware_version+'  FW:'+firmware_date;
//end;
//-----------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------
if (fBuf[0] = $03) then begin // загрузка элемента массива спектра

  address:=         fBuf[1] shl 8;
  address:=address+ fBuf[2];

  massive_element:=                 fBuf[3] shl 24;
  massive_element:=massive_element+(fBuf[4] shl 16);
  massive_element:=massive_element+(fBuf[5] shl 8);
  massive_element:=massive_element+ fBuf[6];


  if (address < max_address - 1) then
  begin
    spectra_massive[address]:=massive_element;
    spectra_massive_ready[address]:=true;

  end
  else
  begin
    SetLength(vAns, 1);
    vAns[0]:=$39;

    Timer3.Enabled:=false;
    Unit1.Form1.Close;

    SaveDialog1.DefaultExt := '.csv';
    SaveDialog1.FileName := 'energy.csv';

    If SaveDialog1.Execute then
    begin
      try
        AssignFile(Fx, SaveDialog1.fileName);  // связали файл с переменной
        Rewrite(Fx);               // создаем пустой файл
        for ixx := 0 to max_address do begin
          WriteLn(Fx,IntToStr(ixx), ',', spectra_massive[ixx]);
        end;
      finally
        CloseFile(Fx);
      end;
    end;
  end;
end;
//-----------------------------------------------------------------------------------


end;
end;
  if Length(vAns) > 0 then RS232.Send(vAns);
end;
//================================================================================================================


// =============================================================================
procedure TmainFrm.DoOnSendedEvent(Sender: TObject; const aData: TiaBuf;
  aCount: Cardinal);
Var
  F: INteger;
  ss: String;
begin
  ss := '';
  For F := 0 To Length(aData) - 1 do
    ss := ss + IntToHex(aData[F],2) + ' ';

end;


end.
