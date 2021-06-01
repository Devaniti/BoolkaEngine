@echo off
pushd "%~dp0"
  call Internal\FindVisualStudioInstallation.bat
  call PrepareScene.bat
  call BuildAndStartReleaseEngine.bat
popd
