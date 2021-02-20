@echo off
call FindVisualStudioInstallation.bat
echo building OBJConverter
%InstallDir%\MSBuild\Current\Bin\MSBuild.exe BoolkaEngine.sln -target:ObjConverter -p:Configuration=Release

if exist Scenes\SanMiguel\ (
  echo SanMiguel scene already present, skip download
) else (
  echo Downloading SanMiguel scene
  echo Models downloaded from Morgan McGuire's Computer Graphics Archive https://casual-effects.com/data
  mkdir Scenes
  mkdir Scenes\SanMiguel
  powershell -command "Start-BitsTransfer -Source https://casual-effects.com/g3d/data10/research/model/San_Miguel/San_Miguel.zip -Destination Scenes/SanMiguel/San_Miguel.zip"
  echo Downloaded scene, unpacking
  powershell -command "Expand-Archive Scenes/SanMiguel/San_Miguel.zip Scenes/SanMiguel/"
  echo Unpacked, deleting archive
  del "Scenes\SanMiguel\San_Miguel.zip"
)

mkdir Scenes\Binarized

echo Binarizing SanMiguel scene
x64\Release\OBJConverter.exe Scenes\SanMiguel\ san-miguel-low-poly.obj ..\Binarized\san-miguel-low-poly.scene
echo Prepared scene successfully
