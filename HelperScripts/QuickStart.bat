@echo off
pushd "%~dp0"
  call PrepareScene.bat || exit /b 1
  call BuildAndStartReleaseEngine.bat || exit /b 1
popd
