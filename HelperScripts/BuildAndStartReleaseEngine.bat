@echo off
pushd "%~dp0"
  call Internal\FindVisualStudioInstallation.bat
  pushd ..
    (%InstallDir%\Common7\IDE\devenv.exe BoolkaEngine.sln /Build "Release" && echo Successfully built Release configuration) || echo Building Release configuration failed && pause && exit /b 1
    pushd x64\Release
      start Bootstrap.exe ..\..\Scenes\Binarized\
    popd
  popd
popd
