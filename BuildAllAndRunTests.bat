@echo off
call FindVisualStudioInstallation.bat
echo Building solution
(%InstallDir%\Common7\IDE\devenv.exe BoolkaEngine.sln /Build "Debug" && echo Successfully built Debug configuration) || echo Building Debug configuration failed && exit /b 1
(%InstallDir%\Common7\IDE\devenv.exe BoolkaEngine.sln /Build "Release" && echo Successfully built Release configuration) || echo Building Release configuration failed && exit /b 1
(%InstallDir%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe x64\Debug\BoolkaCommonUnitTests.dll && echo Tests passed on Debug configuration) || echo Tests failed on Debug configuration && exit /b 1
(%InstallDir%\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe x64\Release\BoolkaCommonUnitTests.dll && echo Tests passed on Release configuration) || echo Tests failed on Release configuration && exit /b 1
