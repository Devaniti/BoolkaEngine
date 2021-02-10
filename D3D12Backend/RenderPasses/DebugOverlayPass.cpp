#include "stdafx.h"
#include "DebugOverlayPass.h"

#ifdef BLK_ENABLE_STATS

#include "RenderSchedule/ResourceTracker.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "APIWrappers/Device.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/backends/imgui_impl_win32.h"
#include "ThirdParty/imgui/backends/imgui_impl_dx12.h"
#include "ThirdParty/imgui/imgui_helper.h"

namespace Boolka
{

    bool DebugOverlayPass::Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        m_ImguiDescriptorHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

        const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(renderContext.GetRenderEngineContext().GetHWND());
        ImGui_ImplDX12_Init(device.Get(), BLK_IN_FLIGHT_FRAMES,
            DXGI_FORMAT_R8G8B8A8_UNORM, m_ImguiDescriptorHeap.Get(),
            m_ImguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            m_ImguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        return true;
    }

    void DebugOverlayPass::Unload()
    {
        const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        m_ImguiDescriptorHeap.Unload();
    }

    bool DebugOverlayPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);

        ImguiFlipFrame();
        ImguiUIManagement(renderContext);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& backbuffer = engineContext.GetSwapchainBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = engineContext.GetSwapchainRenderTargetView(frameIndex);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "DebugOverlayPass");

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE, engineContext.GetDepthStencilView().GetCPUDescriptor());
        ID3D12DescriptorHeap* descriptorHeaps[] = { m_ImguiDescriptorHeap.Get() };
        commandList->SetDescriptorHeaps(ARRAYSIZE(descriptorHeaps), descriptorHeaps);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

        return true;
    }

    bool DebugOverlayPass::PrepareRendering()
    {
        return true;
    }

    void DebugOverlayPass::ImguiFlipFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void DebugOverlayPass::ImguiUIManagement(const RenderContext& renderContext)
    {
        ImGui::Begin("Stats");
        const auto& debugStats = renderContext.GetRenderFrameContext().GetFrameStats();
        const float fps = 1.0f / debugStats.frameTime;
        const auto& cameraPos = renderContext.GetRenderFrameContext().GetCameraPos();
        const auto& viewMatrix = renderContext.GetRenderFrameContext().GetViewMatrix();
        const Vector3 viewDir{ viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2] };
        ImGui::Text("%.1f FPS", fps);
        ImGui::Text("%.2f ms", debugStats.frameTime * 1000.0f);
        if (ImGui::CollapsingHeader("Culling"))
        {
            ImGui::Text("%llu inside frustum", debugStats.insideFrustum);
            ImGui::Text("%llu intersect frustum", debugStats.intersectFrustum);
            ImGui::Text("%llu outside frustum", debugStats.outsideFrustum);
        }
        if (ImGui::CollapsingHeader("Camera"))
        {
            ImGui::Text("Pos X:%.2f Y:%.2f Z:%.2f", cameraPos.x(), cameraPos.y(), cameraPos.z());
            ImGui::Text("Dir X:%.2f Y:%.2f Z:%.2f", viewDir.x(), viewDir.y(), viewDir.z());
        }
        ImGui::End();
        ImGui::Render();
    }

}

#endif
