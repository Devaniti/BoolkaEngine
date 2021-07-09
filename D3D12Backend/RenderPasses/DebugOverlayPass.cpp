#include "stdafx.h"

#include "DebugOverlayPass.h"

#ifdef BLK_ENABLE_STATS

#include "APIWrappers/Device.h"
#include "Contexts/RenderContext.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "RenderSchedule/ResourceTracker.h"
#include "ThirdParty/imgui/backends/imgui_impl_dx12.h"
#include "ThirdParty/imgui/backends/imgui_impl_win32.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_helper.h"

namespace Boolka
{

    bool DebugOverlayPass::Initialize(Device& device, RenderContext& renderContext)
    {
        m_ImguiDescriptorHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                         D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

        const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        // DPI Scaling
        UINT dpi = ::GetDpiForWindow(renderContext.GetRenderEngineContext().GetHWND());
        float scaleFactor = static_cast<float>(dpi) / BLK_WINDOWS_DEFAULT_SCREEN_DPI;
        ImGui::GetStyle().ScaleAllSizes(scaleFactor);
        ImFontConfig fontConfig{};
        fontConfig.SizePixels = 13.0f * scaleFactor;
        ImGui::GetIO().Fonts->AddFontDefault(&fontConfig);

        ImGui_ImplWin32_Init(renderContext.GetRenderEngineContext().GetHWND());
        ImGui_ImplDX12_Init(device.Get(), BLK_IN_FLIGHT_FRAMES, DXGI_FORMAT_R8G8B8A8_UNORM,
                            m_ImguiDescriptorHeap.Get(),
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
        BLK_RENDER_PASS_START(DebugOverlayPass);
        const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);

        ImguiFlipFrame();
        ImguiUIManagement(renderContext);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& backbuffer = resourceContainer.GetBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = resourceContainer.GetBackBufferRTV(frameIndex);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        resourceTracker.Transition(backbuffer, commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->OMSetRenderTargets(1, backbufferRTV.GetCPUDescriptor(), FALSE, nullptr);
        ID3D12DescriptorHeap* descriptorHeaps[] = {m_ImguiDescriptorHeap.Get()};
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
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        ImGui::Begin("Stats");
        const auto& debugStats = frameContext.GetFrameStats();
        const float fps = 1.0f / debugStats.frameTime;
        const float fpsStable = 1.0f / debugStats.frameTimeStable;
        const auto& cameraPos = frameContext.GetCameraPos();
        const auto& viewMatrix = frameContext.GetViewMatrix();
        const Vector3 viewDir{viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]};
        ImGui::Text("%5.1f FPS (avg %5.1f FPS)", fps, fpsStable);
        ImGui::Text("%5.2f ms (avg %5.2f ms)", debugStats.frameTime * 1000.0f,
                    debugStats.frameTimeStable * 1000.0f);
        ImGui::Text("Resolution: %dx%d", engineContext.GetBackbufferWidth(),
                    engineContext.GetBackbufferHeight());
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Pos X:%.2f Y:%.2f Z:%.2f", cameraPos.x(), cameraPos.y(), cameraPos.z());
            ImGui::Text("Dir X:%.2f Y:%.2f Z:%.2f", viewDir.x(), viewDir.y(), viewDir.z());
        }
        if (ImGui::CollapsingHeader("GPU Times", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImguiUIGPUTimes(renderContext);
        }
        if (ImGui::CollapsingHeader("Graphs", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImguiGraphs(renderContext);
        }
        ImGui::End();
        ImGui::Render();
    }

    const char* GetMarkerName(size_t i)
    {
        BLK_ASSERT(i < static_cast<size_t>(TimestampContainer::Markers::Count));

        TimestampContainer::Markers marker = static_cast<TimestampContainer::Markers>(i);
        switch (marker)
        {
        case TimestampContainer::Markers::UpdateRenderPass:
            return "UpdateRenderPass";
            break;
        case TimestampContainer::Markers::ZRenderPass:
            return "ZRenderPass";
            break;
        case TimestampContainer::Markers::ShadowMapRenderPass:
            return "ShadowMapRenderPass";
            break;
        case TimestampContainer::Markers::GBufferRenderPass:
            return "GBufferRenderPass";
            break;
        case TimestampContainer::Markers::ReflectionRenderPass:
            return "ReflectionRenderPass";
            break;
        case TimestampContainer::Markers::DeferredLightingPass:
            return "DeferredLightingPass";
            break;
        case TimestampContainer::Markers::SkyBoxRenderPass:
            return "SkyBoxRenderPass";
            break;
        case TimestampContainer::Markers::TransparentRenderPass:
            return "TransparentRenderPass";
            break;
        case TimestampContainer::Markers::ToneMappingPass:
            return "ToneMappingPass";
            break;
        case TimestampContainer::Markers::DebugOverlayPass:
            return "DebugOverlayPass";
            break;
        case TimestampContainer::Markers::PresentPass:
            return "PresentPass";
            break;
        case TimestampContainer::Markers::EndFrame:
            return "Frame Time";
            break;
        default:
            BLK_ASSERT(0);
            return "ERROR";
            break;
        }
    }

    void DebugOverlayPass::ImguiUIGPUTimes(const RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        const auto& debugStats = frameContext.GetFrameStats();
        const auto& gpuTimes = debugStats.gpuTimes;
        const auto& gpuTimesStable = debugStats.gpuTimesStable;

        if (ImGui::BeginTable("GPUTimes", 4, ImGuiTableFlags_SizingStretchProp))
        {
            for (size_t i = 0; i < ARRAYSIZE(gpuTimes.Markers); ++i)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text(GetMarkerName(i));
                ImGui::TableNextColumn();
                ImGui::Text("%.2fms", 1000.0f * gpuTimes.Markers[i]);
                ImGui::TableNextColumn();
                ImGui::Text("%.2fms", 1000.0f * gpuTimesStable.Markers[i]);
                ImGui::TableNextColumn();
                m_GPUPassGraphs[i].PushValueAndRender(1000.0f * gpuTimes.Markers[i], nullptr, 300, 50);
            }

            ImGui::EndTable();
        }
    }

    void DebugOverlayPass::ImguiGraphs(const RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        const auto& debugStats = frameContext.GetFrameStats();
        const auto& gpuTimes = debugStats.gpuTimes;
        float windowWidth = ImGui::GetWindowSize().x;
        float graphWidth = windowWidth - 32.0f;
        float graphHeight = 80.0f;

        float currentFrameTime = debugStats.frameTime * 1000.0f; // convert to ms
        float currentFPS = 1.0f / debugStats.frameTime;
        float currentGPUTime =
            gpuTimes.Markers[static_cast<size_t>(TimestampContainer::Markers::EndFrame)] *
            1000.0f; // convert to ms

        m_FPSGraph.PushValueAndRender(currentFPS, "FPS", graphWidth, graphHeight);
        m_FrameTimeGraph.PushValueAndRender(currentFrameTime, "Frame time", graphWidth,
                                            graphHeight);
        m_GPUTime.PushValueAndRender(currentGPUTime, "GPU Time", graphWidth, graphHeight);
    }

} // namespace Boolka

#endif
