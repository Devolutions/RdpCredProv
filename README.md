# RdpCredProv

This is an experimental credential provider with fun features for RDP. Use at your own risk. I have built this to get autologon to work with Hyper-V in a lab environment where security is not a concern.

## Building

```powershell
mkdir build-cmake && cd build-cmake
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

## Installation

Copy RdpCredProv.dll to C:\Windows\System32:

```powershell
Copy-Item RdpCredProv.dll "C:\Windows\System32\RdpCredProv.dll" -Force
```

Register it:

```powershell
$RdpCredProvClsid = "{DD2ACC5E-EF4B-4C89-B296-15489C9FAC47}"
$basePath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Authentication\Credential Providers\$RdpCredProvClsid"
New-Item -Path $basePath -Force | Out-Null
Set-ItemProperty -Path $basePath -Name "(default)" -Value "RdpCredProv"
$clsidRegPath = "CLSID\$RdpCredProvClsid"
$inprocPath = "CLSID\$$RdpCredProvClsid\InprocServer32"
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

Those credentials will be used automatically in the Hyper-V enhanced session mode. If you want to enable autologon in the Hyper-V basic session mode, set the following registry keys:

```powershell
$RdpCredProvRegPath = "HKLM:\SOFTWARE\Devolutions\RdpCredProv"
Set-ItemProperty -Path $RdpCredProvRegPath -Name "RemoteOnly" -Value 0 -Type DWORD
$WinlogonRegPath = "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon"
Set-ItemProperty -Path $WinlogonRegPath -Name "DisableCAD" -Value 1 -Type DWORD
```

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
