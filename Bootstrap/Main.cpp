#include "BoolkaCommon/stdafx.h"

#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"
#include "D3D12Backend/RenderBackend.h"
#include "FileReader/FileReader.h"
#include <iostream>

int RealMain(int argc, wchar_t* argv[])
{
    BLK_CRITICAL_ASSERT(argc == 1);

    Boolka::DebugProfileTimer loadTimer;
    loadTimer.Start();

    Boolka::FileReader fileReader;
    if (!fileReader.OpenFile(argv[0]))
        return -1;

    Boolka::SceneData sceneData(fileReader);

    if (!fileReader.StartStreaming(sceneData.GetMemory()))
        return -1;

    Boolka::DebugProfileTimer renderInitTimer;
    renderInitTimer.Start();
    Boolka::RenderBackend* renderer = Boolka::RenderBackend::CreateRenderBackend();
    bool res = renderer->Initialize();
    BLK_CRITICAL_ASSERT(res);
    renderInitTimer.Stop(L"Render initialization");

    Boolka::DebugProfileTimer sceneCreationTimer;
    sceneCreationTimer.Start();
    if (!renderer->LoadScene(sceneData))
        return -1;
    sceneCreationTimer.Stop(L"Scene creation");

    fileReader.CloseFile();
    fileReader.FreeData(sceneData.GetMemory());

    loadTimer.Stop(L"Load");

    ::GetAsyncKeyState(VK_ESCAPE);
    while (true)
    {
        renderer->RenderFrame();
        renderer->Present();

        if (::GetAsyncKeyState(VK_ESCAPE))
            break;
    }
    renderer->UnloadScene();
    renderer->Unload();

    Boolka::RenderBackend::DeleteRenderBackend(renderer);

    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    int argc;
    LPWSTR* argv = ::CommandLineToArgvW(pCmdLine, &argc);
    return RealMain(argc, argv);
}

int wmain(int argc, wchar_t* argv[])
{
    return RealMain(argc, argv);
}
