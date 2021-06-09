pushd "%~dp0"

call Internal\BuildSolution.bat Release || goto error

pushd ..\x64\Release
echo %cd%
start Bootstrap.exe ..\..\Scenes\Binarized\
popd
popd
exit /b 0 

:error
popd
exit /b 1
