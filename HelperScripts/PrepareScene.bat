@echo off
pushd "%~dp0"
call Internal\FindVisualStudioInstallation.bat
pushd ..
echo building OBJConverter
%InstallDir%\MSBuild\Current\Bin\MSBuild.exe BoolkaEngine.sln -m -target:ObjConverter -p:Configuration=Release || echo Error building ObjConverter && goto error

if exist Scenes\SanMiguel\ echo SanMiguel scene already present, skip download && goto skybox
  
echo Downloading SanMiguel scene
echo Models downloaded from Morgan McGuire's Computer Graphics Archive https://casual-effects.com/data
mkdir Scenes
mkdir Scenes\SanMiguel
powershell -command "Start-BitsTransfer -Source http://direct.boolka.dev/files/San_Miguel.zip -Destination Scenes/SanMiguel/San_Miguel.zip"
echo Downloaded scene, unpacking
powershell -command "Expand-Archive Scenes/SanMiguel/San_Miguel.zip Scenes/SanMiguel/"
echo Scene unpacked, deleting archive
del "Scenes\SanMiguel\San_Miguel.zip"

:skybox
if exist Scenes\SanMiguel\skybox echo Skybox already present, skip unpacking && goto binarize

mkdir Scenes\SanMiguel\skybox
powershell -command "Expand-Archive BinaryData/DefaultSkybox.zip Scenes/SanMiguel/skybox/"
echo Skybox unpacked

:binarize
mkdir Scenes\Binarized
echo Binarizing SanMiguel scene
x64\Release\OBJConverter.exe Scenes\SanMiguel\ san-miguel-low-poly.obj ..\Binarized\ || echo Failed to binarize scene && goto error

popd
popd
exit /b 0

:error
popd
popd
exit /b 1
