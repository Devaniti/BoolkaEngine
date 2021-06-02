@echo off
pushd "%~dp0"
  call Internal\FindVisualStudioInstallation.bat
  echo Building solution
  call Internal\BuildSolution.bat Debug || exit /b 1
  call Internal\BuildSolution.bat Development || exit /b 1
  call Internal\BuildSolution.bat Release || exit /b 1
  call Internal\RunTests.bat Debug || exit /b 1
  call Internal\RunTests.bat Development || exit /b 1
  call Internal\RunTests.bat Release || exit /b 1
popd