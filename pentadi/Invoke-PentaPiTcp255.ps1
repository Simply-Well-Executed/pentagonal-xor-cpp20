# SPDX-License-Identifier: 0BSD
# PENTAPI-TCP-255-001: bounded IPv4 TCP loopback transport qualification.
# IPv4 TTL is a hop limit, NOT application retention or years.
# OS TCP stack; no public listener or third-party data.
# TCP byte stream uses explicit unsigned 16-bit framing.
[CmdletBinding()]
param(
 [Parameter(Mandatory=$true)][string]$EvidenceDirectory,
 [ValidateRange(1,255)][int]$InitialIPv4Ttl=255,
 [ValidateRange(500,10000)][int]$TimeoutMilliseconds=2500
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$ev=[IO.Path]::GetFullPath($EvidenceDirectory)
if([IO.Directory]::Exists($ev) -and
   @(Get-ChildItem -LiteralPath $ev -Force).Count -gt 0){
 throw 'PENTAPI_TCP|FAIL|nonempty_evidence_directory'
}
[IO.Directory]::CreateDirectory($ev)|Out-Null
$listener=$null
$client=$null
$server=$null
function SendAll([Net.Sockets.Socket]$socket,[byte[]]$bytes) {
 $offset=0
 while($offset -lt $bytes.Length){
  $count=$socket.Send($bytes,$offset,$bytes.Length-$offset,
                     [Net.Sockets.SocketFlags]::None)
  if($count -le 0){throw 'PENTAPI_TCP|FAIL|send_closed'}
  $offset+=$count
 }
}
function ReceiveExact([Net.Sockets.Socket]$socket,[int]$count){
 if($count -lt 0 -or $count -gt 1024){
  throw 'PENTAPI_TCP|FAIL|invalid_frame_size'
 }
 [byte[]]$result=[byte[]]::new($count)
 $offset=0
 while($offset -lt $count){
  $n=$socket.Receive($result,$offset,$count-$offset,
                     [Net.Sockets.SocketFlags]::None)
  if($n -le 0){throw 'PENTAPI_TCP|FAIL|receive_closed'}
  $offset+=$n
 }
 return ,$result
}
try {
 $loopback=[Net.IPAddress]::Loopback
 $listener=[Net.Sockets.TcpListener]::new($loopback,0)
 $listener.Start(1)
 $port=([Net.IPEndPoint]$listener.LocalEndpoint).Port
 if($port -le 0){throw 'PENTAPI_TCP|FAIL|no_ephemeral_port'}
 $client=[Net.Sockets.Socket]::new([Net.Sockets.AddressFamily]::InterNetwork,
                                    [Net.Sockets.SocketType]::Stream,
                                    [Net.Sockets.ProtocolType]::Tcp)
 if($client.AddressFamily -ne [Net.Sockets.AddressFamily]::InterNetwork -or
    $client.SocketType -ne [Net.Sockets.SocketType]::Stream -or
    $client.ProtocolType -ne [Net.Sockets.ProtocolType]::Tcp){
  throw 'PENTAPI_TCP|FAIL|non_tcp_or_non_ipv4_socket'
 }
 # Set IP TTL BEFORE connection, not after successful TCP connect.
 $client.Ttl=[short]$InitialIPv4Ttl
 if([int]$client.Ttl -ne $InitialIPv4Ttl){
  throw 'PENTAPI_TCP|FAIL|TTL_not_applied_preconnect'
 }
 $client.ReceiveTimeout=$TimeoutMilliseconds
 $client.SendTimeout=$TimeoutMilliseconds
 $client.Connect([Net.IPEndPoint]::new($loopback,$port))
 $server=$listener.AcceptSocket()
 $server.ReceiveTimeout=$TimeoutMilliseconds
 $server.SendTimeout=$TimeoutMilliseconds
 if($client.AddressFamily -ne [Net.Sockets.AddressFamily]::InterNetwork -or
    -not ([Net.IPEndPoint]$client.RemoteEndPoint).Address.Equals($loopback) -or
    -not ([Net.IPEndPoint]$server.LocalEndPoint).Address.Equals($loopback)){
  throw 'PENTAPI_TCP|FAIL|non_loopback_endpoint'
 }
 [byte[]]$payload=[Text.Encoding]::ASCII.GetBytes('PENTAPI|1')
 if($payload.Length -ne 9){throw 'PENTAPI_TCP|FAIL|payload_size'}
 [byte[]]$frame=[byte[]]::new(2+$payload.Length)
 $frame[0]=0
 $frame[1]=[byte]$payload.Length
 [Array]::Copy($payload,0,$frame,2,$payload.Length)
 SendAll $client $frame
 [byte[]]$header=ReceiveExact $server 2
 $size=([int]$header[0] -shl 8) -bor [int]$header[1]
 if($size -ne 9){throw 'PENTAPI_TCP|FAIL|framing_size'}
 [byte[]]$serverPayload=ReceiveExact $server $size
 if([Text.Encoding]::ASCII.GetString($serverPayload) -cne 'PENTAPI|1'){
  throw 'PENTAPI_TCP|FAIL|payload_mismatch'
 }
 SendAll $server $frame
 [byte[]]$echo=ReceiveExact $client $frame.Length
 if([Convert]::ToHexString($echo) -cne [Convert]::ToHexString($frame)){
  throw 'PENTAPI_TCP|FAIL|echo_mismatch'
 }
 if([int]$client.Ttl -ne $InitialIPv4Ttl){
  throw 'PENTAPI_TCP|FAIL|postconnect_TTL_option_mismatch'
 }
 # Local option verification, NOT evidence of actual TTL in a router.
 $tab=[string][char]9
 $lines=[string[]]@(
  ('field'+$tab+'value'),
  ('protocol'+$tab+'TCP_over_IPv4'),
  ('initial_ip_ttl'+$tab+$InitialIPv4Ttl),
  ('scope'+$tab+'127.0.0.1_only'),
  ('transport_framing'+$tab+'16_bit_big_endian_length'),
  ('payload_bytes'+$tab+'9'),
  ('echo_exact'+$tab+'PASS'),
  ('on_wire_ttl_capture'+$tab+'NOT_TESTED'),
  ('timeout_milliseconds'+$tab+$TimeoutMilliseconds),
  ('application_retention_years'+$tab+'NOT_APPLICABLE'),
  ('release'+$tab+'HOLD')
 )
 $path=Join-Path $ev 'PentaPi_TCP_255.tsv'
 [IO.File]::WriteAllLines($path,$lines,[Text.UTF8Encoding]::new($false))
 $hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
 Write-Host ('PENTAPI_TCP|LOOPBACK=PASS|IPV4_TTL='+$InitialIPv4Ttl+
             '|SET_BEFORE_CONNECT=PASS|TCP_FRAMING=PASS|ECHO=PASS'+
             '|ON_WIRE_TTL=NOT_TESTED|NO_EXTERNAL_NETWORK=PASS'+
             '|EVIDENCE_SHA256='+$hash+'|RELEASE=HOLD')
}finally{
 if($server -ne $null){$server.Dispose()}
 if($client -ne $null){$client.Dispose()}
 if($listener -ne $null){$listener.Stop()}
}
