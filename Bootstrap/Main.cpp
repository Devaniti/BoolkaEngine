#include "BoolkaCommon/stdafx.h"

#include <iostream>
#include <thread>

#include "BoolkaCommon/DebugHelpers/DebugProfileTimer.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"
#include "D3D12Backend/ProjectConfig.h"
#include "D3D12Backend/RenderBackend.h"

int RealMain(int argc, wchar_t* argv[])
{
    BLK_CRITICAL_ASSERT(argc == 1);

    if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
    {
        BLK_ASSERT(0);
    }

    Boolka::DebugProfileTimer loadTimer;
    loadTimer.Start();

    Boolka::RenderBackend* renderer = Boolka::RenderBackend::CreateRenderBackend();
    bool res = renderer->Initialize(argv[0]);
    BLK_CRITICAL_ASSERT(res);

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
