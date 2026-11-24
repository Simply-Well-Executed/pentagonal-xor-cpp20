{ SPDX-License-Identifier: 0BSD
  PENTADI-001, Delphi 7 compatible Object Pascal, UNQUALIFIED until
  an authenticated licensed DCC32 compiler passes independent tests.
  Derived from user-provided BranchPinToTsht and table. Never claim
  "i3382.1x-96" is an IEEE specification or actual compiler format.
  Safety delta: {$R+}, {$Q+}, {$I+} on purpose. Do NOT turn off the
  three checks for correctness qualification or branch safety.
}
unit BranchPinToTsht;

{$R+}
{$Q+}
{$I+}

interface

uses SysUtils;

const
  MIN_SIGNED_BYTE = -128;
  MAX_SIGNED_BYTE = 127;
  MASK_16BIT = $FFFF;
  REMAP_TABLE_SIZE = 16;
  DELPHI007_PROFILE_TAG = 'Delphi007';
  I3382_1X_96_TAG = 'i3382.1x-96';
  IEEE_ORG_REF = 'IEEE (https://www.ieee.org/)';

type
  TBranchEncoding = (tSHt, beLong);
  TBranchRangeStatus = (brsInRange, brsOutOfRange);
  TI3382StaticLink = record
    KeyName: string;
    Hash16, Bucket4, OrigIndex, RemapIndex, Payload16: Word;
    NextStaticIdx: SmallInt;
  end;
  TI3382PartitionTable = array[0..REMAP_TABLE_SIZE-1] of TI3382StaticLink;
  TBranchRangeResult = record
    Status: TBranchRangeStatus;
    Disp16: LongInt; // preserves exact (target-next), no saturation
    SignedByteValue: ShortInt;
    FitsInSignedByte: Boolean;
    Low16, High16: Word;
  end;
  TBranchPassRecord = record
    PassNumber: Word;
    Encoding: TBranchEncoding;
    ByteWidth: Word;
  end;
  TBranchHistory = record
    BranchId: Word;
    LabelName: string;
    SourceLine: Word;
    History: array of TBranchPassRecord;
    Oscillating, PinnedToTsht: Boolean;
    PinnedByteWidth: Word;
    LastRangeValid: Boolean;
    LastDisp16: LongInt;
    LastLow16: Word;
  end;
  TBranchOptimizerTracker = class
  private
    FEnabled: Boolean;
    FBranches: array of TBranchHistory;
    FTotalBytesSaved: LongInt;
    FOscillatingCount, FPinnedCount, FRangeErrorCount: Word;
    function FindBranchIndex(BranchId: Word): Integer;
    function FindOrCreateBranch(BranchId: Word;
      const LabelName: string; SourceLine: Word): Integer;
    function HasRepeatedEncoding(const H: TBranchHistory;
      NewEncoding: TBranchEncoding; out PriorPass: Word): Boolean;
  public
    constructor Create(Enabled: Boolean);
    function IsPinnedToTsht(BranchId: Word; out PinnedWidth: Word): Boolean;
    function CheckPinnedTshtRange(BranchId: Word; const LabelName: string;
      SourceLine: Word; NextInstrAddr16, TargetAddr16: Word;
      LogOutOfRange: Boolean; out RangeRes: TBranchRangeResult): Boolean;
    procedure ObserveAndApplyPin(PassNumber: Word; BranchId: Word;
      const LabelName: string; SourceLine: Word;
      var Encoding: TBranchEncoding; var ByteWidth: Word;
      DefaultTshtWidth: Word;
      NextInstrAddr16: Word = 0; TargetAddr16: Word = 0;
      CheckRange: Boolean = False);
    procedure EndPass(PassNumber, RelaxedCount, ExpandedCount: Word;
      BytesSaved16: SmallInt; DurationMs: Double);
    procedure DumpReport;
    property TotalBytesSaved: LongInt read FTotalBytesSaved;
    property OscillatingCount: Word read FOscillatingCount;
    property PinnedCount: Word read FPinnedCount;
    property RangeErrorCount: Word read FRangeErrorCount;
  end;

const
  I3382_1X_96_PARTITION_TABLE: TI3382PartitionTable = (
    (KeyName:'ASM_REL8';Hash16:$15D9;Bucket4:9;OrigIndex:8;RemapIndex:0;Payload16:$9080;NextStaticIdx:1),
    (KeyName:'DNF_EQ_PIV';Hash16:$1977;Bucket4:7;OrigIndex:1;RemapIndex:1;Payload16:$2196;NextStaticIdx:2),
    (KeyName:'DNF_LO_LT';Hash16:$1CA5;Bucket4:5;OrigIndex:0;RemapIndex:2;Payload16:$1338;NextStaticIdx:3),
    (KeyName:'DNF_GT_HI';Hash16:$2464;Bucket4:4;OrigIndex:2;RemapIndex:3;Payload16:$3382;NextStaticIdx:4),
    (KeyName:'LEGO_SHL16';Hash16:$2810;Bucket4:0;OrigIndex:10;RemapIndex:4;Payload16:$B010;NextStaticIdx:5),
    (KeyName:'IEEE_REF';Hash16:$2E7E;Bucket4:14;OrigIndex:15;RemapIndex:5;Payload16:$0EEE;NextStaticIdx:6),
    (KeyName:'C11_MAP_1';Hash16:$69AE;Bucket4:14;OrigIndex:7;RemapIndex:6;Payload16:$8111;NextStaticIdx:7),
    (KeyName:'DELPHI_Q';Hash16:$790C;Bucket4:12;OrigIndex:5;RemapIndex:7;Payload16:$6071;NextStaticIdx:8),
    (KeyName:'DELPHI_R';Hash16:$7DC5;Bucket4:5;OrigIndex:4;RemapIndex:8;Payload16:$5072;NextStaticIdx:9),
    (KeyName:'PASS_GUARD';Hash16:$7E87;Bucket4:7;OrigIndex:14;RemapIndex:9;Payload16:$F010;NextStaticIdx:10),
    (KeyName:'DELPHI_I';Hash16:$9ED4;Bucket4:4;OrigIndex:6;RemapIndex:10;Payload16:$7069;NextStaticIdx:11),
    (KeyName:'LEGO_LOW16';Hash16:$9F5B;Bucket4:11;OrigIndex:9;RemapIndex:11;Payload16:$A0FF;NextStaticIdx:12),
    (KeyName:'REMAP_IDX';Hash16:$A555;Bucket4:5;OrigIndex:13;RemapIndex:12;Payload16:$E00F;NextStaticIdx:13),
    (KeyName:'TSHT_PIN';Hash16:$C08D;Bucket4:13;OrigIndex:3;RemapIndex:13;Payload16:$4016;NextStaticIdx:14),
    (KeyName:'HASH_FNV16';Hash16:$E6B6;Bucket4:6;OrigIndex:12;RemapIndex:14;Payload16:$D16B;NextStaticIdx:15),
    (KeyName:'LEGO_SHR16';Hash16:$E726;Bucket4:6;OrigIndex:11;RemapIndex:15;Payload16:$C020;NextStaticIdx:-1)
  );

function ShaveWord16(Val16: Word): Word;
function Split32To16Bricks(Val32: LongWord; out Low16, High16: Word): Word;
function Join16BricksTo32(Low16, High16: Word): LongWord;
function Hash16Key(const S: string): Word;
function LookupI3382Remap(const KeyName: string;
  out Link: TI3382StaticLink): Boolean;
procedure DNFPartition3Way16(var Arr: array of Word; LoIdx, HiIdx: SmallInt;
  out LtIdx, GtIdx: SmallInt);
procedure DNFQuickSort16(var Arr: array of Word; LoIdx, HiIdx: SmallInt);

implementation

function ShaveWord16(Val16: Word): Word;
begin Result := Val16 and MASK_16BIT; end;

function Split32To16Bricks(Val32: LongWord; out Low16, High16: Word): Word;
begin
  Low16 := Word(Val32 and MASK_16BIT);
  High16 := Word(Val32 shr 16);
  Result := Low16;
end;

function Join16BricksTo32(Low16, High16: Word): LongWord;
begin
  Result := (LongWord(High16) shl 16) or LongWord(Low16);
end;

function Hash16Key(const S: string): Word;
var I: Integer; H: LongWord;
begin
  H := $811C;
  for I := 1 to Length(S) do begin
    H := H xor Ord(S[I]);
    H := (H * $0193) and MASK_16BIT;
  end;
  Result := Word(H);
end;

function LookupI3382Remap(const KeyName: string;
  out Link: TI3382StaticLink): Boolean;
var Target: Word; Lo, Hi, Mid: Integer; Key: string;
begin
  Result := False;
  Key := UpperCase(Trim(KeyName));
  Target := Hash16Key(Key);
  Lo := 0; Hi := REMAP_TABLE_SIZE-1;
  while Lo<=Hi do begin
    Mid := Lo+(Hi-Lo) div 2;
    if I3382_1X_96_PARTITION_TABLE[Mid].Hash16=Target then begin
      if I3382_1X_96_PARTITION_TABLE[Mid].KeyName=Key then begin
        Link := I3382_1X_96_PARTITION_TABLE[Mid];
        Result := True;
      end;
      Exit; // collision is not a false identity
    end;
    if I3382_1X_96_PARTITION_TABLE[Mid].Hash16<Target then
      Lo:=Mid+1 else Hi:=Mid-1;
  end;
end;

procedure SwapWords16(var A, B: Word);
var Tmp: Word;
begin Tmp:=A; A:=B; B:=Tmp; end;

procedure DNFPartition3Way16(var Arr: array of Word; LoIdx, HiIdx: SmallInt;
  out LtIdx, GtIdx: SmallInt);
var Pivot: Word; I: Integer; Lt, Gt: Integer;
begin
  if (LoIdx<0) or (HiIdx<0) or (HiIdx>=Length(Arr)) then
    raise Exception.Create('PENTADI|HOLD|DNF_bounds');
  Lt:=LoIdx; Gt:=HiIdx; I:=LoIdx;
  if LoIdx<HiIdx then begin
    Pivot:=Arr[LoIdx];
    while I<=Gt do begin
      if Arr[I]<Pivot then begin
        SwapWords16(Arr[Lt],Arr[I]); Inc(Lt); Inc(I);
      end else if Arr[I]>Pivot then begin
        SwapWords16(Arr[I],Arr[Gt]); Dec(Gt);
      end else Inc(I);
    end;
  end;
  LtIdx:=SmallInt(Lt); GtIdx:=SmallInt(Gt);
end;

procedure DNFQuickSort16(var Arr: array of Word; LoIdx, HiIdx: SmallInt);
var Lt, Gt: SmallInt;
begin
  if (LoIdx<0) or (HiIdx>=Length(Arr)) then
    raise Exception.Create('PENTADI|HOLD|QS_bounds');
  if LoIdx>=HiIdx then Exit;
  DNFPartition3Way16(Arr,LoIdx,HiIdx,Lt,Gt);
  if Lt>LoIdx then DNFQuickSort16(Arr,LoIdx,Lt-1);
  if Gt<HiIdx then DNFQuickSort16(Arr,Gt+1,HiIdx);
end;

function EncodingToStr(E:TBranchEncoding):string;
begin
  case E of tSHt:Result:='tSHt'; beLong:Result:='beLong';
  else Result:='UNKNOWN'; end;
end;

constructor TBranchOptimizerTracker.Create(Enabled:Boolean);
begin
  inherited Create;
  FEnabled:=Enabled;
  SetLength(FBranches,0);
  FTotalBytesSaved:=0; FOscillatingCount:=0;
  FPinnedCount:=0; FRangeErrorCount:=0;
end;

function TBranchOptimizerTracker.FindBranchIndex(BranchId:Word):Integer;
var I: Integer;
begin
  Result:=-1;
  for I:=0 to High(FBranches) do
    if FBranches[I].BranchId=BranchId then begin Result:=I; Exit; end;
end;

function TBranchOptimizerTracker.FindOrCreateBranch(BranchId:Word;
  const LabelName:string; SourceLine:Word):Integer;
begin
  Result:=FindBranchIndex(BranchId);
  if Result>=0 then Exit;
  if Length(FBranches)>=65536 then
    raise Exception.Create('PENTADI|HOLD|branch_capacity');
  Result:=Length(FBranches);
  SetLength(FBranches,Result+1);
  FBranches[Result].BranchId:=BranchId;
  FBranches[Result].LabelName:=LabelName;
  FBranches[Result].SourceLine:=SourceLine;
  SetLength(FBranches[Result].History,0);
  FBranches[Result].Oscillating:=False;
  FBranches[Result].PinnedToTsht:=False;
  FBranches[Result].PinnedByteWidth:=0;
  FBranches[Result].LastRangeValid:=True;
  FBranches[Result].LastDisp16:=0;
  FBranches[Result].LastLow16:=0;
end;

function TBranchOptimizerTracker.HasRepeatedEncoding(const H:TBranchHistory;
  NewEncoding:TBranchEncoding; out PriorPass:Word):Boolean;
var I, N: Integer;
begin
  Result:=False; PriorPass:=0; N:=Length(H.History);
  if N<2 then Exit;
  if NewEncoding=H.History[N-1].Encoding then Exit;
  for I:=0 to N-2 do
    if H.History[I].Encoding=NewEncoding then begin
      PriorPass:=H.History[I].PassNumber;
      Result:=True; Exit;
    end;
end;

function TBranchOptimizerTracker.IsPinnedToTsht(BranchId:Word;
  out PinnedWidth:Word):Boolean;
var Idx:Integer;
begin
  Idx:=FindBranchIndex(BranchId);
  Result:=(Idx>=0) and FBranches[Idx].PinnedToTsht;
  PinnedWidth:=0;
  if Result then PinnedWidth:=FBranches[Idx].PinnedByteWidth;
end;

function TBranchOptimizerTracker.CheckPinnedTshtRange(BranchId:Word;
  const LabelName:string; SourceLine:Word; NextInstrAddr16,TargetAddr16:Word;
  LogOutOfRange:Boolean; out RangeRes:TBranchRangeResult):Boolean;
var Diff:LongInt;
begin
  { Addresses are unsigned 16-bit LINEAR offsets, no invented wrap.
    Do not saturate or cast the displacement to SmallInt before checking. }
  Diff:=LongInt(TargetAddr16)-LongInt(NextInstrAddr16);
  RangeRes.Disp16:=Diff;
  RangeRes.Low16:=Word(Diff and MASK_16BIT);
  RangeRes.High16:=Word((Diff shr 16) and MASK_16BIT);
  RangeRes.FitsInSignedByte:=(Diff>=MIN_SIGNED_BYTE) and
    (Diff<=MAX_SIGNED_BYTE);
  Result:=RangeRes.FitsInSignedByte;
  RangeRes.SignedByteValue:=0;
  if Result then begin
    RangeRes.Status:=brsInRange;
    RangeRes.SignedByteValue:=ShortInt(Diff);
  end else begin
    RangeRes.Status:=brsOutOfRange;
    Inc(FRangeErrorCount);
    if LogOutOfRange then
      WriteLn(Format('[BRANCH][RANGE_ERR] id=%d line=%d label=%s diff=%d',
        [BranchId,SourceLine,LabelName,Diff]));
  end;
end;

procedure TBranchOptimizerTracker.ObserveAndApplyPin(
  PassNumber,BranchId:Word; const LabelName:string; SourceLine:Word;
  var Encoding:TBranchEncoding; var ByteWidth:Word; DefaultTshtWidth:Word;
  NextInstrAddr16,TargetAddr16:Word; CheckRange:Boolean);
var Idx,N:Integer; Prior:Word; RangeRes:TBranchRangeResult;
    Fits:Boolean;
begin
  if (DefaultTshtWidth=0) or (ByteWidth=0) then
    raise Exception.Create('PENTADI|HOLD|zero_encoding_width');
  Idx:=FindOrCreateBranch(BranchId,LabelName,SourceLine);
  Fits:=False;
  if CheckRange then begin
    Fits:=CheckPinnedTshtRange(BranchId,LabelName,SourceLine,
      NextInstrAddr16,TargetAddr16,FEnabled,RangeRes);
    FBranches[Idx].LastRangeValid:=Fits;
    FBranches[Idx].LastDisp16:=RangeRes.Disp16;
    FBranches[Idx].LastLow16:=RangeRes.Low16;
  end;
  if FBranches[Idx].PinnedToTsht then begin
    if CheckRange and Fits then begin
      Encoding:=tSHt;
      ByteWidth:=FBranches[Idx].PinnedByteWidth;
    end else begin
      { Pin invalidated; preserve prior history and record long fallback. }
      FBranches[Idx].PinnedToTsht:=False;
      Dec(FPinnedCount);
      Encoding:=beLong;
    end;
  end;
  N:=Length(FBranches[Idx].History);
  if (N>0) and (FBranches[Idx].History[N-1].PassNumber=PassNumber) then
    raise Exception.Create('PENTADI|HOLD|duplicate_branch_pass');
  if (not FBranches[Idx].Oscillating) and
     HasRepeatedEncoding(FBranches[Idx],Encoding,Prior) then begin
    FBranches[Idx].Oscillating:=True;
    Inc(FOscillatingCount);
    if FEnabled then
      WriteLn(Format('[BRANCH][OSCILLATE] id=%d pass=%d prior=%d',
        [BranchId,PassNumber,Prior]));
    if CheckRange and Fits then begin
      FBranches[Idx].PinnedToTsht:=True;
      FBranches[Idx].PinnedByteWidth:=DefaultTshtWidth;
      Encoding:=tSHt; ByteWidth:=DefaultTshtWidth;
      Inc(FPinnedCount);
      if FEnabled then WriteLn(Format('[BRANCH][PIN] id=%d width=%d',
        [BranchId,ByteWidth]));
    end;
  end;
  if (Encoding=tSHt) and CheckRange and (not Fits) then
    Encoding:=beLong;
  N:=Length(FBranches[Idx].History);
  SetLength(FBranches[Idx].History,N+1);
  FBranches[Idx].History[N].PassNumber:=PassNumber;
  FBranches[Idx].History[N].Encoding:=Encoding;
  FBranches[Idx].History[N].ByteWidth:=ByteWidth;
end;

procedure TBranchOptimizerTracker.EndPass(
  PassNumber,RelaxedCount,ExpandedCount:Word; BytesSaved16:SmallInt;
  DurationMs:Double);
begin
  FTotalBytesSaved:=FTotalBytesSaved+LongInt(BytesSaved16);
  if FEnabled then WriteLn(Format(
    '[BRANCH][PASS %d] relaxed=%d expanded=%d saved=%d ms=%.2f',
    [PassNumber,RelaxedCount,ExpandedCount,BytesSaved16,DurationMs]));
end;

procedure TBranchOptimizerTracker.DumpReport;
var I,J:Integer; S:string;
begin
  if not FEnabled then Exit;
  WriteLn('--- PENTADI | Delphi007 / i3382.1x-96 | EXPLORATORY ---');
  WriteLn(Format('[BRANCH][SUMMARY] total_saved=%d oscillating=%d pinned=%d range_errors=%d',
    [FTotalBytesSaved,FOscillatingCount,FPinnedCount,FRangeErrorCount]));
  for I:=0 to High(FBranches) do begin
    S:='';
    for J:=0 to High(FBranches[I].History) do begin
      if J>0 then S:=S+' -> ';
      S:=S+Format('P%d:%s(%dB)',
        [FBranches[I].History[J].PassNumber,
         EncodingToStr(FBranches[I].History[J].Encoding),
         FBranches[I].History[J].ByteWidth]);
    end;
    WriteLn(Format('[BRANCH][HISTORY] id=%d pinned=%s range_ok=%s %s',
      [FBranches[I].BranchId,BoolToStr(FBranches[I].PinnedToTsht,True),
       BoolToStr(FBranches[I].LastRangeValid,True),S]));
  end;
end;

end.
