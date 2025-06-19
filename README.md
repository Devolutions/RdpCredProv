# RdpCredProv

This is an experimental credential provider with fun features for RDP. Use at your own risk. I have built this to get autologon to work with Hyper-V in a lab environment where security is not a concern.

## Building

```powershell
mkdir build-cmake && cd build-cmake
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

## Installation

Download and copy RdpCredProv.dll to C:\Windows\System32:

```powershell
Copy-Item RdpCredProv.dll "C:\Windows\System32\RdpCredProv.dll" -Force
```

You can also use the following PowerShell code snippet to download and copy the latest version of RdpCredProv:

```powershell
$ProgressPreference = "SilentlyContinue"
$NativeArch = if ($Env:PROCESSOR_ARCHITECTURE -eq 'ARM64') { "arm64" } else { "x64" }
$RdpCredProvVersion = (Invoke-RestMethod "https://api.github.com/repos/Devolutions/RdpCredProv/releases/latest").tag_name.TrimStart("v")
$RdpCredProvUrl = "https://github.com/Devolutions/RdpCredProv/releases/download/v$RdpCredProvVersion/RdpCredProv-$RdpCredProvVersion-$NativeArch.zip"
Invoke-WebRequest -UseBasicParsing -Uri $RdpCredProvUrl -OutFile "RdpCredProv.zip"
$TempExtractPath = Join-Path $Env:TEMP "RdpCredProv"
Expand-Archive -Path ".\RdpCredProv.zip" -DestinationPath $TempExtractPath
Copy-Item "$TempExtractPath\RdpCredProv.dll" "$Env:SystemRoot\System32\RdpCredProv.dll" -Force
Remove-Item -Path $TempExtractPath -Recurse | Out-Null
Remove-Item "RdpCredProv.zip" | Out-Null
```

Then register the credential provider:

```powershell
$RdpCredProvClsid = "{DD2ACC5E-EF4B-4C89-B296-15489C9FAC47}"
$basePath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Authentication\Credential Providers\$RdpCredProvClsid"
New-Item -Path $basePath -Force | Out-Null
Set-ItemProperty -Path $basePath -Name "(default)" -Value "RdpCredProv"
$clsidRegPath = "CLSID\$RdpCredProvClsid"
$inprocPath = "CLSID\$RdpCredProvClsid\InprocServer32"
$regHKCR = [Microsoft.Win32.Registry]::ClassesRoot
$cpKey = $regHKCR.CreateSubKey($clsidRegPath)
$cpKey.SetValue("", "RdpCredProv")
$inprocKey = $regHKCR.CreateSubKey($inprocPath)
$inprocKey.SetValue("", "RdpCredProv.dll")
$inprocKey.SetValue("ThreadingModel", "Apartment")
$cpKey.Close()
$inprocKey.Close()
```

## Configuration

Set default credentials to be used:

```powershell
$RdpCredProvRegPath = "HKLM:\SOFTWARE\Devolutions\RdpCredProv"
New-Item -Path $RdpCredProvRegPath -Force | Out-Null
Set-ItemProperty -Path $RdpCredProvRegPath -Name "DefaultUserName" -Value "Administrator"
Set-ItemProperty -Path $RdpCredProvRegPath -Name "DefaultPassword" -Value "LabUser123!"
Set-ItemProperty -Path $RdpCredProvRegPath -Name "DefaultDomainName" -Value "."
Set-ItemProperty -Path $RdpCredProvRegPath -Name "AutoLogonWithDefault" -Value 1 -Type DWORD
Set-ItemProperty -Path $RdpCredProvRegPath -Name "UseDefaultCredentials" -Value 1 -Type DWORD
```

Those credentials will be used automatically in the Hyper-V enhanced session mode, and inside a RDP session with RDP NLA disabled. Here are a few example user names with their correct mapping for reference:

**Administrator** (local account):
 * DefaultUserName: "Administrator"
 * DefaultDomainName: "."

**IT-HELP\Administrator** (domain account):
 * DefaultUserName: "Administrator"
 * DefaultDomainName: "IT-HELP"

**Administrator@ad.it-help.ninja** (domain account):
 * DefaultUserName: "Administrator@ad.it-help.ninja"
 * DefaultDomainName: ""

### Console Session

If you want to enable autologon in the Hyper-V basic session mode, or with the physical (console) session, set **RemoteOnly** to **0**:

```powershell
$RdpCredProvRegPath = "HKLM:\SOFTWARE\Devolutions\RdpCredProv"
Set-ItemProperty -Path $RdpCredProvRegPath -Name "RemoteOnly" -Value 0 -Type DWORD
```

For a completely automatic logon, disable the Ctrl+Alt+Del (CAD) secure access sequence:

```powershell
$WinlogonRegPath = "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon"
Set-ItemProperty -Path $WinlogonRegPath -Name "DisableCAD" -Value 1 -Type DWORD
$SystemPoliciesRegPath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System"
Set-ItemProperty -Path $SystemPoliciesRegPath -Name "DisableCAD" -Value 1 -Type DWORD
```

### RDP without NLA

For regular RDP, a credential provider can only *complement* RDP NLA but not *substitute* it. The only way to really perform autologon in RDP where the client sends no credentials is to disable RDP NLA on the client and server.

With mstsc.exe, save your .RDP file (Default.rdp) and add or edit the following line to it: `enablecredsspsupport:i:0`.

In the RDP server, ensure RDP NLA enforcement is disabled:

```powershell
Set-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\Terminal Server\WinStations\RDP-Tcp' -Name 'UserAuthentication' -Value 0
```

You should now be able to connect with no credentials sent from the client, with a server that will perform automatic logon with the saved credentials.

## RDP with NLA

RDP NLA performs NTLM or Kerberos authentication *before* the full credentials are delegated to the server and sent to Winlogon. This means it is unfortunately not possible for the server to just connect without authenticating, and have the credential provider perform autologon on behalf of the client. The only thing the credential provider can do is customize the authentication with additional steps, but it cannot fully replace it. For this reason, the current credential provider does nothing useful with RDP NLA.

## Logging

To enable logging, set the **LogEnabled** registry key. The log files will be located in "%ProgramData%\RdpCredProv":

```powershell
$RdpCredProvRegPath = "HKLM:\SOFTWARE\Devolutions\RdpCredProv"
Set-ItemProperty -Path $RdpCredProvRegPath -Name "LogEnabled" -Value 1 -Type DWORD
```

## Uninstallation

Unregister the credential provider:

```powershell
$RdpCredProvClsid = "{DD2ACC5E-EF4B-4C89-B296-15489C9FAC47}"
$basePath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Authentication\Credential Providers\$RdpCredProvClsid"
$clsidRegPath = "CLSID\$RdpCredProvClsid"
if (Test-Path $basePath) {
    Remove-Item -Path $basePath -Recurse -Force
}
try {
    $regHKCR = [Microsoft.Win32.Registry]::ClassesRoot
    $regHKCR.DeleteSubKeyTree($clsidRegPath)
} catch { }
```

Reboot the machine or kill Winlogon.exe, then delete RdpCredProv.dll:

```powershell
Get-Process Winlogon | Stop-Process -Force
Get-Item "C:\Windows\System32\RdpCredProv.dll" | Remove-Item
```
