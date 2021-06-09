@echo off
pushd "%~dp0"

echo Building solution
call Internal\BuildSolution.bat Debug || goto error
call Internal\BuildSolution.bat Development || goto error
call Internal\BuildSolution.bat Release || goto error
call Internal\RunTests.bat Debug || goto error
call Internal\RunTests.bat Development || goto error
call Internal\RunTests.bat Release || goto error
  
popd
exit /b 0

:error
popd
exit /b 1