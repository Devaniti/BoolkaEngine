@echo off
pushd "%~dp0"
  call FindVisualStudioInstallation.bat
  pushd ..\..
    (%InstallDir%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe x64\%1\BoolkaCommonUnitTests.dll && echo Tests passed on %1 configuration) || echo Tests failed on %1 configuration && pause && exit /b 1
  popd
popd
