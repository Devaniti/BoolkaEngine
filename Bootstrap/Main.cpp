#include "BoolkaCommon/stdafx.h"

#include <iostream>
#include <thread>

#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"
#include "D3D12Backend/Containers/RenderCacheContainer.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"
#include "D3D12Backend/ProjectConfig.h"
#include "D3D12Backend/RenderBackend.h"
#include "FileReader/FileReader.h"

int RealMain(int argc, wchar_t* argv[])
{
    BLK_CRITICAL_ASSERT(argc == 1);

    Boolka::DebugProfileTimer loadTimer;
    loadTimer.Start();

    Boolka::RenderCacheContainer renderCache;
#ifdef BLK_ENABLE_PIPELINE_LIBRARY_LOAD_FROM_DISK
    if (renderCache.PSOCacheReader.OpenFile(BLK_PSO_CACHE_FILENAME, true))
    {
        renderCache.PSOCacheReader.StartStreaming(renderCache.PSOCache);
    }
#endif
    // TODO implement RTAS caching
    // currently that file is never written/used
    if (renderCache.RTCacheReader.OpenFile(BLK_RT_CACHE_FILENAME, true))
    {
        renderCache.RTCacheReader.StartStreaming(renderCache.RTCache);
    }

    Boolka::FileReader sceneFileReader;
    std::wstring scenePath;
    //CombinePath(argv[0], BLK_SCENE_REQUIRED_SCENE_DATA_FILENAME, scenePath);

    //if (!sceneFileReader.OpenFile(scenePath.c_str()))
    //    return -1;

    Boolka::SceneData sceneData(sceneFileReader);

    //if (!sceneFileReader.StartStreaming(sceneData.GetMemory()))
    //    return -1;

    Boolka::DebugProfileTimer renderInitTimer;
    renderInitTimer.Start();
    Boolka::RenderBackend* renderer = Boolka::RenderBackend::CreateRenderBackend();
    bool res = renderer->Initialize(renderCache);
    BLK_CRITICAL_ASSERT(res);
    renderInitTimer.Stop(L"Render initialization");

    Boolka::DebugProfileTimer sceneCreationTimer;
    sceneCreationTimer.Start();
    if (!renderer->LoadScene(sceneData))
        return -1;
    sceneCreationTimer.Stop(L"Scene creation");

    //sceneFileReader.CloseFile();
    //sceneFileReader.FreeData(sceneData.GetMemory());

    loadTimer.Stop(L"Load");

    //::GetAsyncKeyState(VK_ESCAPE);
    //while (true)
    //{
    //    renderer->RenderFrame();
    //    renderer->Present();

    //    if (::GetAsyncKeyState(VK_ESCAPE))
    //        break;
    //}
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
