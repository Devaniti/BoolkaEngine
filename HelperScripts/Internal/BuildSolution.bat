@echo off
pushd "%~dp0"
call FindVisualStudioInstallation.bat
pushd ..\..
(%InstallDir%\MSBuild\Current\Bin\MSBuild.exe BoolkaEngine.sln -m -t:Restore -p:RestorePackagesConfig=true;Configuration=%1 && echo Successfully restored packages for %1 configuration) || echo Failed to restore Nuget packages on %1 configuration failed && goto error
(%InstallDir%\MSBuild\Current\Bin\MSBuild.exe BoolkaEngine.sln -m -t:Build -p:Configuration=%1 && echo Successfully built %1 configuration) || echo Failed to build %1 configuration && goto error

:success
popd
popd
exit /b 0

:error
popd
popd
exit /b 1