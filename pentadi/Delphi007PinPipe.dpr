{ SPDX-License-Identifier: 0BSD
  PENTADI-001, optional licensed Delphi 7 adapter, not yet DCC32-tested.
  Only clean C11 pipe-delimited records are consumed. No external lookup,
  no floating point, no silent default to a valid address on parse failure.
}
program Delphi007PinPipe;

{$APPTYPE CONSOLE}
{$R+}
{$Q+}
{$I+}

uses SysUtils, BranchPinToTsht, BuildSpeedProfiler;

function SplitPipeLine(const Line:string; var Parts:array of string):Integer;
var I,StartIdx,FieldIdx:Integer;
begin
  for I:=0 to High(Parts) do Parts[I]:='';
  StartIdx:=1;
  FieldIdx:=0;
  for I:=1 to Length(Line) do
    if Line[I]='|' then begin
      if FieldIdx>High(Parts) then
        raise Exception.Create('PENTADI|HOLD|too_many_fields');
      Parts[FieldIdx]:=Copy(Line,StartIdx,I-StartIdx);
      Inc(FieldIdx);
      StartIdx:=I+1;
    end;
  if FieldIdx>High(Parts) then
    raise Exception.Create('PENTADI|HOLD|too_many_fields');
  Parts[FieldIdx]:=Copy(Line,StartIdx,Length(Line)-StartIdx+1);
  Result:=FieldIdx+1;
end;

function ParseWord(const S:string):Word;
var V:Integer;
begin
  if (not TryStrToInt(Trim(S),V)) or (V<0) or (V>65535) then
    raise Exception.Create('PENTADI|HOLD|invalid_unsigned_16');
  Result:=Word(V);
end;

procedure ValidateRemap;
var I,J:Integer; Link:TI3382StaticLink;
    Seen:array[0..15] of Boolean; Keys:array[0..6] of Word;
begin
  for I:=0 to 15 do Seen[I]:=False;
  for I:=0 to REMAP_TABLE_SIZE-1 do begin
    Link:=I3382_1X_96_PARTITION_TABLE[I];
    if Link.Hash16<>Hash16Key(Link.KeyName) then
      raise Exception.Create('PENTADI|HOLD|hash_mismatch');
    if Link.Bucket4<>(Link.Hash16 and 15) then
      raise Exception.Create('PENTADI|HOLD|bucket_mismatch');
    if (Link.OrigIndex>15) or Seen[Link.OrigIndex] then
      raise Exception.Create('PENTADI|HOLD|duplicate_origin');
    Seen[Link.OrigIndex]:=True;
    if Link.RemapIndex<>I then
      raise Exception.Create('PENTADI|HOLD|remap_mismatch');
    if I<15 then begin
      if Link.NextStaticIdx<>I+1 then
        raise Exception.Create('PENTADI|HOLD|static_chain_mismatch');
    end else if Link.NextStaticIdx<>-1 then
      raise Exception.Create('PENTADI|HOLD|terminal_chain_mismatch');
    if not LookupI3382Remap(Link.KeyName,Link) then
      raise Exception.Create('PENTADI|HOLD|lookup_failed');
  end;
  if LookupI3382Remap('SOME_UNREGISTERED_KEY',Link) then
    raise Exception.Create('PENTADI|HOLD|false_positive_lookup');
  Keys[0]:=9;Keys[1]:=1;Keys[2]:=9;Keys[3]:=0;
  Keys[4]:=65535;Keys[5]:=1;Keys[6]:=7;
  DNFQuickSort16(Keys,0,6);
  for J:=1 to 6 do
    if Keys[J]<Keys[J-1] then
      raise Exception.Create('PENTADI|HOLD|unsorted_DNF');
  if (Keys[0]<>0) or (Keys[6]<>65535) then
    raise Exception.Create('PENTADI|HOLD|DNF_boundary');
end;

var
 Tracker:TBranchOptimizerTracker;
 Line:string;
 Parts:array[0..7] of string;
 N,PassNum,Id,SrcLine,Width,NextAddr,Target:Word;
 Enc:TBranchEncoding;
 Count:Integer;
 Profiler:TSpeedProfiler;
 Proposed,Qualified:TAsmCompilerOptions;
 ResolvedPath:string;
begin
 Tracker:=nil;
 Profiler:=nil;
 try
   { Preserve user's speed proposal as data, never disable runtime guards. }
   ConfigureBuildOptions(bpFinal,Proposed);
   if QualifiedBuildSafe(Proposed) then
     raise Exception.Create('PENTADI|HOLD|speed_profile_control_drift');
   Qualified:=Proposed;
   ApplyQualifiedSafety(Qualified);
   if not QualifiedBuildSafe(Qualified) then
     raise Exception.Create('PENTADI|HOLD|qualified_safety_failed');
   WriteLn('PENTADI_PROFILE|USER_FINAL_SPEED=EXPLORATORY_UNSAFE|QUALIFIED_RANGE_OVERFLOW_IO=PASS|RELEASE=HOLD');
   Profiler:=TSpeedProfiler.Create(True);
   Profiler.BeginStage('i3382-DNF-hash-remap');
   ValidateRemap;
   Profiler.EndStage;
   Profiler.BeginStage('C11-branch-stream');
   Tracker:=TBranchOptimizerTracker.Create(True);
   Count:=0;
   while not EOF(Input) do begin
     ReadLn(Input,Line);
     Line:=Trim(Line);
     if Line='' then Continue;
     if Line[1]='#' then Continue;
     N:=Word(SplitPipeLine(Line,Parts));
     if N<>8 then
       raise Exception.Create('PENTADI|HOLD|record_fields_not_8');
     PassNum:=ParseWord(Parts[0]);
     Id:=ParseWord(Parts[1]);
     SrcLine:=ParseWord(Parts[2]);
     Width:=ParseWord(Parts[5]);
     NextAddr:=ParseWord(Parts[6]);
     Target:=ParseWord(Parts[7]);
     if (PassNum=0) or (Id=0) or (Width=0) then
       raise Exception.Create('PENTADI|HOLD|zero_identifier_or_width');
     if SameText(Trim(Parts[4]),'LONG') then
       Enc:=beLong
     else if SameText(Trim(Parts[4]),'SHORT') then
       Enc:=tSHt
     else
       raise Exception.Create('PENTADI|HOLD|unknown_encoding');
     if (Length(Trim(Parts[3]))=0) or (Length(Parts[3])>64) then
       raise Exception.Create('PENTADI|HOLD|invalid_label');
     Tracker.ObserveAndApplyPin(PassNum,Id,Trim(Parts[3]),SrcLine,
       Enc,Width,2,NextAddr,Target,True);
     Inc(Count);
   end;
   Profiler.EndStage;
   if not Profiler.ResolveSearchPath('BranchPinToTsht.pas',
     [GetCurrentDir],ResolvedPath) then
     raise Exception.Create('PENTADI|HOLD|profiler_source_probe_failed');
   Profiler.BeginStage('branch-report');
   Tracker.DumpReport;
   Profiler.EndStage;
   Profiler.DumpReport;
   if (Count<>9) or (Tracker.OscillatingCount<>1) or
      (Tracker.PinnedCount<>1) or (Tracker.RangeErrorCount<>4) then
     raise Exception.Create('PENTADI|HOLD|witness_counts');
   WriteLn('PENTADI_DELPHI7|RECORDS=9|OSCILLATIONS=1|PINS=1|RANGE_REJECTED=4|DNF=PASS|REMAP=PASS|RELEASE=HOLD');
 except
   on E:Exception do begin
     WriteLn('PENTADI_DELPHI7|HOLD|'+E.Message);
     if Profiler<>nil then Profiler.Free;
     if Tracker<>nil then Tracker.Free;
     Halt(2);
   end;
 end;
 Profiler.Free;
 Tracker.Free;
end.
