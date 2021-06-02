@echo off
pushd "%~dp0"
  call FindVisualStudioInstallation.bat
  pushd ..\..
    (%InstallDir%\MSBuild\Current\Bin\MSBuild.exe BoolkaEngine.sln -t:restore -p:RestorePackagesConfig=true;Configuration=%1 && echo Successfully restored packages for %1 configuration) || echo Failed to restore Nuget packages on %1 configuration failed && pause && exit /b 1
    (start /wait "" %InstallDir%\Common7\IDE\devenv.exe BoolkaEngine.sln /Build "%1" && echo Successfully built %1 configuration) || echo Failed to build %1 configuration && pause && exit /b 1
  popd
popd
