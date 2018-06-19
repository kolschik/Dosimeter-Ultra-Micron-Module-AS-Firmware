unit Unit1;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, JvExControls, JvChart, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.Grids;

type
  TForm1 = class(TForm)
    JvChart1: TJvChart;
    Channel: TEdit;
    Label2: TLabel;
    Window: TEdit;
    Label3: TLabel;
    Found: TEdit;
    Timer1: TTimer;
    Button1: TButton;
    Label4: TLabel;
    Rep_coefficent: TEdit;
    Label5: TLabel;
    Rep_save: TButton;
    Button2: TButton;
    Timer_dreif: TTimer;
    dreif: TButton;
    SaveDialog1: TSaveDialog;
    StringGrid1: TStringGrid;
    procedure FormCreate(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure Rep_saveClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure dreifClick(Sender: TObject);
    procedure Timer_dreifTimer(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    dreif_mass: array [0 .. 2048] of UInt32;
    dreif_pointer: UInt32;
  end;

var
  Form1: TForm1;
  coefficent: Extended;
   procedure Mesh_Array(var In_mass: array of UInt32; Rep_calc_coefficent: Extended);
implementation

{$R *.dfm}
uses main;

// =============================================================================




// -------------------------------------------------------------------------------------------------------------
// Масштабирование массива на коэфицент
// Неебическая магия!!! ОПАСНО!!!
// -------------------------------------------------------------------------------------------------------------
procedure Mesh_Array(var In_mass: array of UInt32; Rep_calc_coefficent: Extended);
var
  address_pointer,how_many_transfer,i,i2: uint32;
  pointer,part_to_sample: Extended;
  tmp_mass: array [0 .. 4096] of UInt32;
begin

// -----------------------------------------------------
for i := 0 to max_address do // очистка временного массива
begin
  tmp_mass[i]:=0;
end;
// -----------------------------------------------------

address_pointer:=0;
pointer:=0;

if Rep_calc_coefficent=0 then Rep_calc_coefficent :=1;

part_to_sample:=1/Rep_calc_coefficent;

// -----------------------------------------------------
if Rep_calc_coefficent>1 then // если график надо растянуть
begin

  for i := 0 to max_address do
  begin

    if Trunc(pointer+part_to_sample)>Trunc(pointer) then // Если конец окна выходит за придел ячейки
    begin

      how_many_transfer:=Trunc(In_mass[Trunc(pointer)]*(Trunc(pointer+part_to_sample)-pointer));
      In_mass[Trunc(pointer)]:=In_mass[Trunc(pointer)]-how_many_transfer;

      if In_mass[Trunc(pointer)]>0 then how_many_transfer:=how_many_transfer+In_mass[Trunc(pointer)];

      pointer:=pointer+part_to_sample;

      how_many_transfer:=how_many_transfer+Trunc(In_mass[Trunc(pointer)]*Frac(pointer));
      In_mass[Trunc(pointer)]:=In_mass[Trunc(pointer)]-Trunc(In_mass[Trunc(pointer)]*Frac(pointer));

    end else
    begin

      how_many_transfer:=Trunc(In_mass[Trunc(pointer)]*part_to_sample);

      In_mass[Trunc(pointer)]:=In_mass[Trunc(pointer)]-how_many_transfer;
      if In_mass[Trunc(pointer)]>0 then how_many_transfer:=how_many_transfer+In_mass[Trunc(pointer)];

      pointer:=pointer+part_to_sample;

    end;

      tmp_mass[i]:=how_many_transfer;
  end;

end else // Если график надо сжать
begin

  for i := 0 to max_address do
  begin
    if Trunc(pointer+part_to_sample)<max_address then
    begin
      if Trunc(pointer+part_to_sample)>Trunc(pointer)+1 then // Если конец окна выходит за придел следующей ячейки
      begin
        how_many_transfer:=                    Trunc(In_mass[Trunc(pointer)]*(Trunc(pointer+1)-pointer)); // Остаток от текущей ячейки
        how_many_transfer:=how_many_transfer + Trunc(In_mass[Trunc(pointer+1)]); // Остаток от следующей ячейки
        pointer:=pointer+part_to_sample;
        how_many_transfer:=how_many_transfer + Trunc(In_mass[Trunc(pointer)]*Frac(pointer)); // Остаток от ячейки через одну
      end else
      begin
        how_many_transfer:=                    Trunc(In_mass[Trunc(pointer)]*(Trunc(pointer+1)-pointer)); // Остаток от текущей ячейки
        pointer:=pointer+part_to_sample;
        how_many_transfer:=how_many_transfer + Trunc(In_mass[Trunc(pointer)]*Frac(pointer)); // Остаток от следующей ячейки

      end;

    end else
    begin
      how_many_transfer:=0;
    end;

    tmp_mass[i]:=how_many_transfer;
  end;

end;

// -----------------------------------------------------
for i := 0 to max_address do // сохранение массива
begin
  In_mass[i]:=tmp_mass[i];
end;
// -----------------------------------------------------

end;
// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------



procedure TForm1.Button1Click(Sender: TObject);
var
i: uint32;
begin

   mainFrm.clear_device();

  // -----------------------------------------------------
  for i := 0 to max_address do // отрисовка графика
  begin
    Unit1.Form1.JvChart1.Data.Value[0,i]:=0;
    Unit1.Form1.JvChart1.Data.Value[1,i]:=0;
    Unit1.Form1.JvChart1.Data.Value[2,i]:=0;
  end;
  // -----------------------------------------------------
end;

procedure TForm1.Button2Click(Sender: TObject);
begin

stab_coefficent:=1;
spectra_massive_multipile_coefficent[spectra_massive_multipile_address+1]:=1;
mainFrm.SaveReg;

end;

procedure TForm1.dreifClick(Sender: TObject);
var
i: uint32;
Fx: TextFile;
FileName: string;
begin

for i := 0 to  2047 do
  Unit1.Form1.StringGrid1.Cells[0, i]:='';

Unit1.Form1.StringGrid1.Cells[0, 0]:='Канал';

 if Unit1.Form1.Timer_dreif.Enabled then
 begin
  Unit1.Form1.Timer_dreif.Enabled:=false;
  Unit1.Form1.dreif.Caption:='Начать анализ дрейфа';
  SaveDialog1.DefaultExt := '.csv';
  SaveDialog1.FileName := 'Dreif_'+mainFrm.Spectro_time.Text+'.csv';
  If SaveDialog1.Execute then
  begin
   try
    AssignFile(Fx, SaveDialog1.FileName); // связали файл с переменной
    Rewrite(Fx); // создаем пустой файл
    for i := 0 to  dreif_pointer do
    begin
      WriteLn(Fx, IntToStr(dreif_mass[i]));
    end;
   finally
    CloseFile(Fx);
   end;
  end;

 end
 else
 begin
  Unit1.Form1.Timer_dreif.Enabled:=true;
   dreif_pointer:=0;
   Unit1.Form1.dreif.Caption:='Остановить анализ дрейфа';
 end;

end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin

    Unit1.Form1.Timer_dreif.Enabled:=false;
    Timer1.Enabled:=false;
    spectra_massive_multipile_address:=spectra_massive_multipile_address+1;
    mainFrm.clear_device();
    sleep(1100);
    mainFrm.clear_device();;
    sleep(1100);
    mainFrm.clear_device();;
    sleep(1100);
    spectra_massive_multipile_enable_write:=true;
end;

procedure TForm1.FormCreate(Sender: TObject);
var
i: uint32;
begin

  Unit1.Form1.JvChart1.Options.XEnd:=2048;
  Unit1.Form1.JvChart1.Options.PenColor[0]:=clRed;
  Unit1.Form1.JvChart1.Options.PenStyle[0]:=psSolid;

  Unit1.Form1.JvChart1.Options.PenColor[1]:=clGreen;
  Unit1.Form1.JvChart1.Options.PenStyle[1]:=psSolid;
  Timer1.Enabled:=true;

  // -----------------------------------------------------
  for i := 0 to max_address do // отрисовка графика
  begin
    Unit1.Form1.JvChart1.Data.Value[0,i]:=0;
    Unit1.Form1.JvChart1.Data.Value[1,i]:=0;
    Unit1.Form1.JvChart1.Data.Value[2,i]:=0;
  end;
  // -----------------------------------------------------


end;

procedure TForm1.Rep_saveClick(Sender: TObject);
var
  i: uint32;
  tmp_massive: array [0 .. 4096] of UInt32;
begin

stab_calibr:=StrToInt(Unit1.Form1.Channel.Text);
stab_window:=StrToInt(Unit1.Form1.Window.Text);
stab_coefficent:=coefficent;

spectra_massive_multipile_coefficent[spectra_massive_multipile_address+1]:=coefficent;
mainFrm.SaveReg;

// -----------------------------------------------------
for i := 0 to max_address do // очистка графика и перенос данных во временный массив
begin
  Unit1.Form1.JvChart1.Data.Value[2,i]:=0;
  tmp_massive[i]:=raw_spectra_massive[i];
end;
// -----------------------------------------------------

Mesh_Array(tmp_massive,coefficent);


// -----------------------------------------------------
for i := 0 to max_address do // отрисовка графика
begin
  Unit1.Form1.JvChart1.Data.Value[2,i]:=tmp_massive[i];
end;
// -----------------------------------------------------


end;

procedure TForm1.Timer1Timer(Sender: TObject);
Var
  i,x,address, found_address: UInt32;
  max: Double;
begin
  max:=0;
  found_address:=0;


  if (Unit1.Form1.Channel.Text='') then Exit;
  if (Unit1.Form1.Window.Text='') then Exit;

  if (Unit1.Form1.Channel.Text='0') then Unit1.Form1.Channel.Text:=IntToStr(stab_calibr);
  if (Unit1.Form1.Window.Text='0') then Unit1.Form1.Window.Text:=IntToStr(stab_window);


  if StrToInt(Unit1.Form1.Channel.Text) > StrToInt(Unit1.Form1.Window.Text)  then
  begin

    for i := 0 to max_address do  Unit1.Form1.JvChart1.Data.Value[1,i]:=0;

    address:=StrToInt(Unit1.Form1.Channel.Text)-StrToInt(Unit1.Form1.Window.Text);

    for i := 0 to (StrToInt(Unit1.Form1.Window.Text)*2) do
    begin
      if Unit1.Form1.JvChart1.Data.Value[0,address+i] > max  then
      begin
        max:= Unit1.Form1.JvChart1.Data.Value[0,address+i];
        found_address:=address+i;
      end;

    end;


    Unit1.Form1.JvChart1.Data.Value[1,found_address]:=max;

    Unit1.Form1.Found.Text:=IntToStr(found_address);

    if found_address > 0 then
    begin
      coefficent:= StrToInt(Unit1.Form1.Channel.Text);
      coefficent:= coefficent / found_address;
    end;

    Unit1.Form1.Rep_coefficent.Text:=FloatToStrF(coefficent, ffFixed, 10, 4);

  end;

  Unit1.Form1.JvChart1.Options.PrimaryYAxis.YMax:=0;

  for i := 0 to max_address do
    if Unit1.Form1.JvChart1.Data.Value[0,i]>Unit1.Form1.JvChart1.Options.PrimaryYAxis.YMax then
      Unit1.Form1.JvChart1.Options.PrimaryYAxis.YMax:=Unit1.Form1.JvChart1.Data.Value[0,i];

   Unit1.Form1.JvChart1.Options.PrimaryYAxis.YMax:=Unit1.Form1.JvChart1.Options.PrimaryYAxis.YMax*1.3;
   Unit1.Form1.JvChart1.PlotGraph;

end;

procedure TForm1.Timer_dreifTimer(Sender: TObject);
begin


  dreif_mass[dreif_pointer]:=StrToInt(Unit1.Form1.Found.Text);
  Unit1.Form1.StringGrid1.Cells[0, dreif_pointer+1]:=Unit1.Form1.Found.Text;

  mainFrm.clear_device();
  sleep(500);
  dreif_pointer:=dreif_pointer+1;

end;

end.
