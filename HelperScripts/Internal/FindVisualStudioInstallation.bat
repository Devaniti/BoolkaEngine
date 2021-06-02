@echo off

if defined InstallDir (
  exit /b 0
)

echo Searching for Visual Studio intallation
set InstallDir=
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do (
  set InstallDir="%%i"
)

if %InstallDir%=="" (
  echo Visual studio not found
  exit /b 1
)

echo Using Visual Studio from path: %InstallDir%