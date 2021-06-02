pushd "%~dp0"
  (call Internal\BuildSolution.bat Release) || exit /b 1
  pushd ..\x64\Release
    echo %cd%
    start Bootstrap.exe ..\..\Scenes\Binarized\
  popd
popd
