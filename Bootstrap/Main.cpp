#include "BoolkaCommon/stdafx.h"
#include "D3D12Backend/RenderBackend.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"
#include "FileReader/FileReader.h"
#include "BoolkaCommon/DebugHelpers/DebugTimer.h"

#include <iostream>

int RealMain()
{
    Boolka::DebugTimer loadTimer;
    loadTimer.Start();

    Boolka::SceneData sceneData;

    Boolka::FileReader fileReader;
    //if (!fileReader.OpenFile(L"C:\\Projects\\san-miguel\\san-miguel-low-poly.scene"))
    if (!fileReader.OpenFile(L"C:\\Projects\\Sponza\\sponza.scene"))
        return -1;

    if (!fileReader.StartStreaming(sceneData.GetMemory()))
        return -1;

    Boolka::RenderBackend* renderer = Boolka::RenderBackend::CreateRenderBackend();
    bool res = renderer->Initialize();
    BLK_CRITICAL_ASSERT(res);

    if (!fileReader.WaitData())
        return -1;

    fileReader.CloseFile();

    if (!renderer->LoadScene(sceneData))
        return -1;

    fileReader.FreeData(sceneData.GetMemory());

    float loadTime = loadTimer.Stop() * 1000.0f;

    char buffer[256];
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

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    return RealMain();
}

int main()
{
    return RealMain();
}
