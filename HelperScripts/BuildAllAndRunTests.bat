@echo off
pushd "%~dp0"
  call Internal\FindVisualStudioInstallation.bat
  pushd ..
    echo Building solution
    (%InstallDir%\Common7\IDE\devenv.exe BoolkaEngine.sln /Build "Debug" && echo Successfully built Debug configuration) || echo Building Debug configuration failed && pause && exit /b 1
    (%InstallDir%\Common7\IDE\devenv.exe BoolkaEngine.sln /Build "Development" && echo Successfully built Development configuration) || echo Building Development configuration failed && pause && exit /b 1
    (%InstallDir%\Common7\IDE\devenv.exe BoolkaEngine.sln /Build "Release" && echo Successfully built Release configuration) || echo Building Release configuration failed && pause && exit /b 1
    (%InstallDir%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe x64\Debug\BoolkaCommonUnitTests.dll && echo Tests passed on Debug configuration) || echo Tests failed on Debug configuration && pause && exit /b 1
    (%InstallDir%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe x64\Development\BoolkaCommonUnitTests.dll && echo Tests passed on Development configuration) || echo Tests failed on Development configuration && pause && exit /b 1
    (%InstallDir%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe x64\Release\BoolkaCommonUnitTests.dll && echo Tests passed on Release configuration) || echo Tests failed on Release configuration && pause && exit /b 1
  popd
popd