#include "BoolkaCommon/stdafx.h"
#include "D3D12Backend/RenderBackend.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"
#include "FileReader/FileReader.h"
#include "BoolkaCommon/DebugHelpers/DebugTimer.h"

#include <iostream>

int RealMain(int argc, wchar_t* argv[])
{
    BLK_CRITICAL_ASSERT(argc == 1);

    Boolka::DebugTimer loadTimer;
    loadTimer.Start();

    Boolka::FileReader fileReader;
    if (!fileReader.OpenFile(argv[0]))
        return -1;

    Boolka::SceneData sceneData(fileReader);

    if (!fileReader.StartStreaming(sceneData.GetMemory()))
        return -1;

    Boolka::RenderBackend* renderer = Boolka::RenderBackend::CreateRenderBackend();
    bool res = renderer->Initialize();
    BLK_CRITICAL_ASSERT(res);

    Boolka::DebugTimer sceneCreationTimer;
    sceneCreationTimer.Start();
    if (!renderer->LoadScene(sceneData))
        return -1;
    float sceneCreationTime = sceneCreationTimer.Stop() * 1000.0f;

    fileReader.CloseFile();

    char buffer[256];

    snprintf(buffer, 256, "Scene creation time:%.3fms\n", sceneCreationTime);
    OutputDebugStringA(buffer);

    fileReader.FreeData(sceneData.GetMemory());

    float loadTime = loadTimer.Stop() * 1000.0f;

    snprintf(buffer, 256, "Load time:%.3fms\n", loadTime);
    OutputDebugStringA(buffer);

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

int main(int argc, wchar_t* argv[])
{
    return RealMain(argc, argv);
}
