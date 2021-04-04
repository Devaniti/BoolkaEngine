BoolkaEngine
============
BoolkaEngine is DX12 rendering engine.

![Screenshot](https://raw.githubusercontent.com/Devaniti/BoolkaEngine/master/Screenshot.png)

Features
--------
* HDR Rendering + Tonemapping
* Point lights with shadows
* Sun light with shadows
* GPU Frustum culling using mesh shaders
* Fast loading using custom binary format (including converted from obj format)
* Skybox
* ImGui debug output

Requirements
--------
* Windows 10 version 2004 or newer
* GPU with mesh shader support (NVIDIA GTX 16xx or higher, AMD RX 6xxx or higher)
* SSD Recommended

To build BoolkaEngine you'll also need:
* Visual Studio 2019
* Windows 10 SDK 10.0.19041

Building
--------
You can just build BoolkaEngine.sln solution, but you'll also need binarized scene to run it

If you want to use default scene run PrepareScene.bat, which will download and binarize San Miguel scene and default skybox

If you want to use another scene:
1. Build OBJConverter project
2. Place your obj, mtl and texture files in one folder (which is going to be inObjFolder for OBJConverter)
3. Create "skybox" subfolder there and place skybox textures (see BinaryData\DefaultSkybox.zip for format reference)
4. Run OBJConverter

Command line parameters
--------
Bootstrap parameters:\
Bootstrap.exe binarizedScenePath

OBJConverter parameters:\
OBJConverter.exe inObjFolder inObjFile outBinarizedFile