@echo off
pushd "%~dp0"
call PrepareScene.bat || goto error
call BuildAndStartReleaseEngine.bat || goto error
popd
exit /b 0

:error
popd
exit /b 1
