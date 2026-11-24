{ SPDX-License-Identifier: 0BSD
  Derived from user-supplied keep.pass (retained verbatim alongside).
  This is a build-profile/measurement unit, not a Delphi compiler.
  Qualification uses compiler-level R+, Q+, I+ checks.
  Any bpFinal unsafe toggles are historical proposed settings only and
  must be overridden for validating branch bounds, memory and I/O.
}
unit BuildSpeedProfiler;

interface

uses
  SysUtils;

type
  TBuildProfile = (bpDev, bpFinal);

  TAsmCompilerOptions = record
    EmitListing: Boolean;
    EmitRelocations: Boolean;
    StrictOperands: Boolean;
    CaseSensitiveLabels: Boolean;
    FoldConstants: Boolean;
    OptimizeBranches: Boolean;
    WarnUnusedSymbols: Boolean;
    TreatWarningsAsErrors: Boolean;
    VerboseDiagnostics: Boolean;

    // Delphi 7 speed / final-build toggles
    Delphi7DebugInfo: Boolean;       // {$D-}
    Delphi7LocalSymbols: Boolean;    // {$L-}
    Delphi7RangeChecks: Boolean;     // {$R-}
    Delphi7OverflowChecks: Boolean;  // {$Q-}
    Delphi7IOChecks: Boolean;        // {$I-}
    Delphi7StackFrames: Boolean;     // {$W-}
    Delphi7MapFile: Boolean;         // No .MAP file output
    DebugBuildSpeedIssues: Boolean;  // Log stage timings & search-path lookups
  end;

  TSpeedProfiler = class
  private
    FEnabled: Boolean;
    FCurrentStageName: string;
    FStageStart: TDateTime;
    FTotalStart: TDateTime;
    FTotalStageMs: Double;
    FSearchPathCount: Integer;
    FSearchMissCount: Integer;
    FSearchElapsedMs: Double;
    function ElapsedMsSince(const StartTime: TDateTime): Double;
    procedure FlushCurrentStage;
  public
    constructor Create(Enabled: Boolean);
    destructor Destroy; override;

    procedure BeginStage(const StageName: string);
    procedure EndStage;

    // Resolves FileName across SearchPaths (e.g., semicolon-delimited or string array)
    // and logs lookup probes/misses when FEnabled is True.
    function ResolveSearchPath(const FileName: string; const SearchPaths: array of string;
      out ResolvedPath: string): Boolean;

    procedure DumpReport;
  end;

procedure ApplyFinalBuildSpeedHacks(var Opts: TAsmCompilerOptions);
procedure ConfigureBuildOptions(Profile: TBuildProfile; out Opts: TAsmCompilerOptions);
procedure ApplyQualifiedSafety(var Opts: TAsmCompilerOptions);
function QualifiedBuildSafe(const Opts: TAsmCompilerOptions): Boolean;

implementation

procedure ApplyQualifiedSafety(var Opts: TAsmCompilerOptions);
begin
  Opts.TreatWarningsAsErrors := True;
  Opts.Delphi7RangeChecks := True;
  Opts.Delphi7OverflowChecks := True;
  Opts.Delphi7IOChecks := True;
  Opts.StrictOperands := True;
  Opts.VerboseDiagnostics := True;
end;

function QualifiedBuildSafe(const Opts: TAsmCompilerOptions): Boolean;
begin
  Result := Opts.TreatWarningsAsErrors and
    Opts.Delphi7RangeChecks and Opts.Delphi7OverflowChecks and
    Opts.Delphi7IOChecks and Opts.StrictOperands and
    Opts.VerboseDiagnostics;
end;

const
  MS_PER_DAY = 24.0 * 60.0 * 60.0 * 1000.0;

procedure ApplyFinalBuildSpeedHacks(var Opts: TAsmCompilerOptions);
begin
  Opts.TreatWarningsAsErrors := False;

  Opts.Delphi7DebugInfo := False;
  Opts.Delphi7LocalSymbols := False;
  Opts.Delphi7RangeChecks := False;
  Opts.Delphi7OverflowChecks := False;
  Opts.Delphi7IOChecks := False;
  Opts.Delphi7StackFrames := False;
  Opts.Delphi7MapFile := False;

  Opts.DebugBuildSpeedIssues := True;
end;

procedure ConfigureBuildOptions(Profile: TBuildProfile; out Opts: TAsmCompilerOptions);
begin
  Opts.EmitListing := True;
  Opts.EmitRelocations := True;
  Opts.StrictOperands := True;
  Opts.CaseSensitiveLabels := True;
  Opts.FoldConstants := False;
  Opts.OptimizeBranches := True;
  Opts.WarnUnusedSymbols := True;
  Opts.VerboseDiagnostics := True;

  if Profile = bpFinal then
    ApplyFinalBuildSpeedHacks(Opts)
  else
  begin
    Opts.TreatWarningsAsErrors := True;

    Opts.Delphi7DebugInfo := True;
    Opts.Delphi7LocalSymbols := True;
    Opts.Delphi7RangeChecks := True;
    Opts.Delphi7OverflowChecks := True;
    Opts.Delphi7IOChecks := True;
    Opts.Delphi7StackFrames := True;
    Opts.Delphi7MapFile := True;
    Opts.DebugBuildSpeedIssues := False;
  end;
end;

{ TSpeedProfiler }

constructor TSpeedProfiler.Create(Enabled: Boolean);
begin
  inherited Create;
  FEnabled := Enabled;
  FCurrentStageName := '';
  FTotalStart := Now;
  FTotalStageMs := 0.0;
  FSearchPathCount := 0;
  FSearchMissCount := 0;
  FSearchElapsedMs := 0.0;
end;

destructor TSpeedProfiler.Destroy;
begin
  FlushCurrentStage;
  inherited Destroy;
end;

function TSpeedProfiler.ElapsedMsSince(const StartTime: TDateTime): Double;
begin
  Result := (Now - StartTime) * MS_PER_DAY;
end;

procedure TSpeedProfiler.FlushCurrentStage;
var
  Elapsed: Double;
begin
  if FCurrentStageName <> '' then
  begin
    Elapsed := ElapsedMsSince(FStageStart);
    FTotalStageMs := FTotalStageMs + Elapsed;
    if FEnabled then
      WriteLn(Format('[SPEED][STAGE] %-28s %8.2f ms', [FCurrentStageName, Elapsed]));
    FCurrentStageName := '';
  end;
end;

procedure TSpeedProfiler.BeginStage(const StageName: string);
begin
  FlushCurrentStage;
  FCurrentStageName := StageName;
  FStageStart := Now;
end;

procedure TSpeedProfiler.EndStage;
begin
  FlushCurrentStage;
end;

function TSpeedProfiler.ResolveSearchPath(const FileName: string;
  const SearchPaths: array of string; out ResolvedPath: string): Boolean;
var
  I, Probes: Integer;
  Candidate, CleanDir: string;
  LookupStart, LookupMs: Double;
begin
  Result := False;
  ResolvedPath := '';

  if not FEnabled then
  begin
    // Fast path when profiling is off: still resolve the file without log overhead
    if FileExists(FileName) then
    begin
      ResolvedPath := ExpandFileName(FileName);
      Result := True;
      Exit;
    end;
    for I := Low(SearchPaths) to High(SearchPaths) do
    begin
      if Trim(SearchPaths[I]) = '' then
        Continue;
      CleanDir := IncludeTrailingPathDelimiter(Trim(SearchPaths[I]));
      Candidate := CleanDir + FileName;
      if FileExists(Candidate) then
      begin
        ResolvedPath := ExpandFileName(Candidate);
        Result := True;
        Exit;
      end;
    end;
    Exit;
  end;

  Inc(FSearchPathCount);
  LookupStart := Now;
  Probes := 1; { the initial FileExists(FileName) probe also counts }

  if FileExists(FileName) then
  begin
    ResolvedPath := ExpandFileName(FileName);
    LookupMs := ElapsedMsSince(LookupStart);
    FSearchElapsedMs := FSearchElapsedMs + LookupMs;
    WriteLn(Format('[SPEED][PATH] HIT  "%s" -> "%s" (%d probe, %.2f ms)',
      [FileName, ResolvedPath, Probes, LookupMs]));
    Result := True;
    Exit;
  end;

  for I := Low(SearchPaths) to High(SearchPaths) do
  begin
    if Trim(SearchPaths[I]) = '' then
      Continue;
    Inc(Probes);
    CleanDir := IncludeTrailingPathDelimiter(Trim(SearchPaths[I]));
    Candidate := CleanDir + FileName;
    if FileExists(Candidate) then
    begin
      ResolvedPath := ExpandFileName(Candidate);
      LookupMs := ElapsedMsSince(LookupStart);
      FSearchElapsedMs := FSearchElapsedMs + LookupMs;
      WriteLn(Format('[SPEED][PATH] HIT  "%s" -> "%s" (%d probes, %.2f ms)',
        [FileName, ResolvedPath, Probes, LookupMs]));
      Result := True;
      Exit;
    end;
  end;

  Inc(FSearchMissCount);
  LookupMs := ElapsedMsSince(LookupStart);
  FSearchElapsedMs := FSearchElapsedMs + LookupMs;
  WriteLn(Format('[SPEED][PATH] MISS "%s" across %d search dir(s) (%.2f ms)',
    [FileName, Length(SearchPaths), LookupMs]));
end;

procedure TSpeedProfiler.DumpReport;
var
  WallMs: Double;
begin
  FlushCurrentStage;
  if not FEnabled then
    Exit;

  WallMs := ElapsedMsSince(FTotalStart);
  WriteLn('--- FINAL Build Speed Diagnostics ---');
  WriteLn(Format('Total wall time      : %.2f ms', [WallMs]));
  WriteLn(Format('Measured stage sum   : %.2f ms', [FTotalStageMs]));
  WriteLn(Format('Path lookups         : %d total (%d misses), %.2f ms spent probing',
    [FSearchPathCount, FSearchMissCount, FSearchElapsedMs]));
  WriteLn('-----------------------------------');
end;

end.
