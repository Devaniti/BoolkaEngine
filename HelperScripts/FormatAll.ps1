$ScriptPath = $MyInvocation.MyCommand.Path
$ScriptDir = Split-Path $ScriptPath
Push-Location $ScriptDir\..

Get-ChildItem -Path ('BoolkaCommon', 'BoolkaCommonUnitTests', 'Bootstrap', 'D3D12Backend', 'OBJConverter') -include ('*.cpp', '*.h', '*.hpp') -Recurse |
    foreach {
		Write-Host "Formatting " $_
        clang-format -i $_
    }

Pop-Location