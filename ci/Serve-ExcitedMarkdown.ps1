# SPDX-License-Identifier: 0BSD
# EXCITED local opt-in read-only Markdown HTTP API. NEVER started by CI.
# Bind only 127.0.0.1; one fixed file; bounded requests; no source writes.
[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)][string]$MarkdownPath,
  [ValidateRange(1024,65535)][int]$Port=8179,
  [ValidateRange(1,100)][int]$MaxRequests=9
)
Set-StrictMode -Version 2.0
$ErrorActionPreference='Stop'
$path=[IO.Path]::GetFullPath($MarkdownPath)
if(-not [IO.File]::Exists($path) -or
   -not [string]::Equals([IO.Path]::GetExtension($path),'.md',[StringComparison]::OrdinalIgnoreCase)){
  throw 'EXCITED_API|HOLD|markdown_source_required'
}
$file=Get-Item -LiteralPath $path -Force
if(($file.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0 -or
   $file.Length -lt 1 -or $file.Length -gt 1048576){
  throw 'EXCITED_API|HOLD|unsafe_markdown'
}
# Load in memory once, immutable per local server session.
$payload=[IO.File]::ReadAllBytes($path)
$expected=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
$tcp=[Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback,$Port)
$tcp.Start(1)
Write-Host ('EXCITED_API|LOCAL_ONLY|http://127.0.0.1:'+$Port+'/v1/markdown|SOURCE_SHA256='+$expected)
try {
  for([int]$handled=0;$handled -lt $MaxRequests;$handled++){
    $client=$tcp.AcceptTcpClient()
    try {
      $client.ReceiveTimeout=3000
      $client.SendTimeout=3000
      $stream=$client.GetStream()
      $request=[Collections.Generic.List[byte]]::new()
      $last4=[Collections.Generic.Queue[byte]]::new()
      $done=$false
      while($request.Count -lt 8192 -and -not $done){
        $b=$stream.ReadByte()
        if($b -lt 0){break}
        $request.Add([byte]$b)
        $last4.Enqueue([byte]$b)
        if($last4.Count -gt 4){$null=$last4.Dequeue()}
        if($last4.Count -eq 4 -and
           [string]::Join(',',@($last4.ToArray())) -ceq '13,10,13,10'){
          $done=$true
        }
      }
      $line=([Text.Encoding]::ASCII.GetString($request.ToArray()) -split "`r`n",2)[0]
      $ok=$done -and $line -ceq 'GET /v1/markdown HTTP/1.1'
      $health=$done -and $line -ceq 'GET /healthz HTTP/1.1'
      if($ok){
        $code='200 OK'
        $type='text/markdown; charset=utf-8'
        $content=$payload
      }elseif($health){
        $code='200 OK'
        $type='application/json; charset=utf-8'
        $content=[Text.Encoding]::UTF8.GetBytes('{"status":"ok","scope":"loopback","release":"HOLD"}')
      }else{
        $code='404 Not Found'
        $type='text/plain; charset=utf-8'
        $content=[Text.Encoding]::UTF8.GetBytes('Not found')
      }
      $header="HTTP/1.1 $code`r`nContent-Type: $type`r`nContent-Length: $($content.Length)`r`nConnection: close`r`nCache-Control: no-store`r`nX-Content-Type-Options: nosniff`r`n`r`n"
      $head=[Text.Encoding]::ASCII.GetBytes($header)
      $stream.Write($head,0,$head.Length)
      $stream.Write($content,0,$content.Length)
      $stream.Flush()
    }finally{$client.Dispose()}
  }
}finally{$tcp.Stop()}
Write-Host 'EXCITED_API|STOPPED|NO_SOURCE_MUTATION|RELEASE=HOLD'
