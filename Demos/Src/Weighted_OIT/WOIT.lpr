program WOIT;

{$AppType Console}
{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, vampyreimagingpackage, untMain, ContextSwitcher;

{$R *.res}

begin
  WriteLn('Hold mouse right button for rotate');
  RequireDerivedFormResource := True;
  Application.Initialize;
  Application.CreateForm(TfrmMain, frmMain);
  Application.Run;
end.

