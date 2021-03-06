BoolkaEngine
============
BoolkaEngine is DX12 rendering engine for Windows 10.

![Screenshot](https://raw.githubusercontent.com/Devaniti/BoolkaEngine/master/ScreenshotSmall.png)

Currently implemented features:
* HDR Rendering + Tonemapping
* Point lights with shadows
* Sun light with shadows
* Frustum culling
* Fast loading using custom binary format (including converted from obj format)
* ImGui debug output

Building
--------
Building BoolkaEngine requires Visual Studio 2017 (2019 recommended) and Windows SDK 10.0.19041 or later.

If you want to use default scene, run PrepareScene.bat, which will download and convert to .scene format San Miguel scene.
