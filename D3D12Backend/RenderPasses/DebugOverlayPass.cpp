#include "stdafx.h"

#include "DebugOverlayPass.h"

#ifdef BLK_ENABLE_STATS

#include "ThirdParty/imgui/backends/imgui_impl_dx12.h"
#include "ThirdParty/imgui/backends/imgui_impl_win32.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_helper.h"

namespace Boolka
{

    DebugOverlayPass::DebugOverlayPass()
        : m_ScaleFactor(0.0f)
        , m_IsEnabled(true)
    {
    }

    DebugOverlayPass::~DebugOverlayPass()
    {
        BLK_ASSERT(m_ScaleFactor == 0.0f);
    }

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
        m_ScaleFactor = static_cast<float>(dpi) / BLK_WINDOWS_DEFAULT_SCREEN_DPI;
        ImGui::GetStyle().ScaleAllSizes(m_ScaleFactor);
        ImFontConfig fontConfig{};
        fontConfig.SizePixels = 13.0f * m_ScaleFactor;
        ImGui::GetIO().Fonts->AddFontDefault(&fontConfig);

        ImGui_ImplWin32_Init(renderContext.GetRenderEngineContext().GetHWND());
        ImGui_ImplDX12_Init(device.Get(), BLK_IN_FLIGHT_FRAMES, DXGI_FORMAT_R8G8B8A8_UNORM,
                            m_ImguiDescriptorHeap.Get(),
                            m_ImguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                            m_ImguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

        m_GPUSupportsRaytracing = device.SupportsRaytracing();
        return true;
    }

    void DebugOverlayPass::Unload()
    {
        const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        m_ImguiDescriptorHeap.Unload();
        m_ScaleFactor = 0.0f;
    }

    bool DebugOverlayPass::Render(RenderContext& renderContext, ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(DebugOverlayPass);
        const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);

        // TODO implement proper input when GDK GameInput releases
        static bool isF1Pressed = false;
        if (::GetAsyncKeyState(VK_F1))
        {
            if (!isF1Pressed)
            {
                m_IsEnabled = !m_IsEnabled;
                isF1Pressed = true;
            }
        }
        else
        {
            isF1Pressed = false;
        }

        if (!m_IsEnabled)
        {
            return true;
        }

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

    const char* GetViewName(size_t viewIndex)
    {
        const char* names[BLK_RENDER_VIEW_COUNT] = {
            "Camera",         "Sun Shadowmap",  "Light 0 Face 0", "Light 0 Face 1",
            "Light 0 Face 2", "Light 0 Face 3", "Light 0 Face 4", "Light 0 Face 5",
            "Light 1 Face 0", "Light 1 Face 1", "Light 1 Face 2", "Light 1 Face 3",
            "Light 1 Face 4", "Light 1 Face 5", "Light 2 Face 0", "Light 2 Face 1",
            "Light 2 Face 2", "Light 2 Face 3", "Light 2 Face 4", "Light 2 Face 5",
            "Light 3 Face 0", "Light 3 Face 1", "Light 3 Face 2", "Light 3 Face 3",
            "Light 3 Face 4", "Light 3 Face 5",
        };

        if (viewIndex < BLK_RENDER_VIEW_COUNT)
        {
            return names[viewIndex];
        }

        BLK_ASSERT(0);
        return "";
    }

    void DebugOverlayPass::ImguiUIManagement(const RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        ImguiStatsWindow(renderContext);
        ImguiDebugWindow(renderContext);
        ImguiHelpWindow();
        ImguiHardwareWindow();

        ImGui::Render();
    }

    void DebugOverlayPass::ImguiHelpWindow()
    {
        ImGui::Begin("Help");
        if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("WASD - Camera movement");
            ImGui::Text("Arrows - Camera rotation");
            ImGui::Text("T - Increase FOV");
            ImGui::Text("R - Decrease FOV");
            ImGui::Text("F1 - Hide/Show UI");
            ImGui::Text("O - Output camera position to clipboard");
            ImGui::Text("L - Load camera position from clipboard");
            ImGui::Text("Esc - Exit");
        }
        ImGui::End();
    }

    void DebugOverlayPass::ImguiDebugWindow(const RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        const auto& cameraPos = frameContext.GetCameraPos();
        const auto& viewMatrix = frameContext.GetViewMatrix();
        const auto& projMatrix = frameContext.GetProjMatrix();

        const Vector3 viewDir{viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]};
        // FOV along Y axis
        const float fov = BLK_RAD_TO_DEG(2.0f * std::atan(1.0f / projMatrix[1][1]));

        ImGui::Begin("Debug");
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Pos X:%.2f Y:%.2f Z:%.2f", cameraPos.x(), cameraPos.y(), cameraPos.z());
            ImGui::Text("Dir X:%.2f Y:%.2f Z:%.2f", viewDir.x(), viewDir.y(), viewDir.z());
            ImGui::Text("FOV %.2f degrees", fov);
        }
        if (ImGui::CollapsingHeader("GPU Debug Markers", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImguiGPUDebugMarkers(renderContext);
        }
        ImGui::End();
    }

    void DebugOverlayPass::ImguiStatsWindow(const RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        ImGui::Begin("Stats");
        const auto& debugStats = frameContext.GetFrameStats();
        const float fps = 1.0f / debugStats.frameTime;
        const float fpsStable = 1.0f / debugStats.frameTimeStable;

        ImGui::Text("%5.1f FPS (avg %5.1f FPS)", fps, fpsStable);
        ImGui::Text("%5.2f ms (avg %5.2f ms)", debugStats.frameTime * 1000.0f,
                    debugStats.frameTimeStable * 1000.0f);
        ImGui::Text("Resolution: %dx%d", engineContext.GetBackbufferWidth(),
                    engineContext.GetBackbufferHeight());
        if (ImGui::CollapsingHeader("Culling", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImguiCullingTable(renderContext);
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
    }

    void DebugOverlayPass::ImguiHardwareWindow()
{
        ImGui::Begin("Hardware");

        ImGui::Text("GPU");
        ImGui::Text("Supports Raytracing - %s", m_GPUSupportsRaytracing ? "true" : "false");
        ImGui::End();
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
        case TimestampContainer::Markers::GPUCullingRenderPass:
            return "GPUCullingRenderPass";
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
        case TimestampContainer::Markers::RaytraceRenderPass:
            return "RaytraceRenderPass";
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

        const float graphWidth = 150.0f * m_ScaleFactor;
        const float graphHeight = 25.0f * m_ScaleFactor;

        if (ImGui::BeginTable("GPUTimes", 5,
                              ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Idx", ImGuiTableColumnFlags_NoSortDescending);
            ImGui::TableSetupColumn("Marker", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Time");
            ImGui::TableSetupColumn("Stable Time");
            ImGui::TableSetupColumn("Graph", ImGuiTableColumnFlags_NoSort);
            ImGui::TableHeadersRow();

            size_t markerIndicies[ARRAYSIZE(gpuTimes.Markers)];
            for (size_t i = 0; i < ARRAYSIZE(gpuTimes.Markers); ++i)
            {
                markerIndicies[i] = i;
            }

            // Sort rows if needed
            const ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
            BLK_ASSERT(sortSpecs->SpecsCount <= 1);

            if (sortSpecs->SpecsCount > 0)
            {
                size_t columnIndex = sortSpecs->Specs[0].ColumnIndex;
                bool reversed = sortSpecs->Specs[0].SortDirection != ImGuiSortDirection_Ascending;

                switch (columnIndex)
                {
                case 0: // NOOP, array already sorted by index
                    break;
                case 2:
                    std::ranges::sort(markerIndicies,
                                      [reversed, &gpuTimes](size_t a, size_t b) -> bool {
                                          if (reversed)
                                          {
                                              return gpuTimes.Markers[a] > gpuTimes.Markers[b];
                                          }
                                          return gpuTimes.Markers[a] < gpuTimes.Markers[b];
                                      });
                    break;
                case 3:
                    std::ranges::sort(
                        markerIndicies, [reversed, &gpuTimesStable](size_t a, size_t b) -> bool {
                            if (reversed)
                            {
                                return gpuTimesStable.Markers[a] > gpuTimesStable.Markers[b];
                            }
                            return gpuTimesStable.Markers[a] < gpuTimesStable.Markers[b];
                        });
                    break;
                }
            }

            for (size_t i = 0; i < ARRAYSIZE(gpuTimes.Markers); ++i)
            {
                size_t index = markerIndicies[i];
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", index);
                ImGui::TableNextColumn();
                ImGui::Text(GetMarkerName(index));
                ImGui::TableNextColumn();
                ImGui::Text("%5.2fms", 1000.0f * gpuTimes.Markers[index]);
                ImGui::TableNextColumn();
                ImGui::Text("%5.2fms", 1000.0f * gpuTimesStable.Markers[index]);
                ImGui::TableNextColumn();
                m_GPUPassGraphs[index].PushValueAndRender(1000.0f * gpuTimes.Markers[index],
                                                          nullptr, graphWidth, graphHeight);
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
        float graphWidth = windowWidth - 16.0f * m_ScaleFactor;
        float graphHeight = 40.0f * m_ScaleFactor;

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

    template <size_t tupleIndex, typename TupleType>
    struct TupleSorter
    {
        bool reversed;
        bool operator()(const TupleType& a, const TupleType& b) const
        {
            if (reversed)
            {
                return std::get<tupleIndex>(a) > std::get<tupleIndex>(b);
            }
            return std::get<tupleIndex>(a) < std::get<tupleIndex>(b);
        }
    };

    void DebugOverlayPass::ImguiCullingTable(const RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        const auto& debugStats = frameContext.GetFrameStats();
        if (ImGui::BeginTable("Visible objects", 4,
                              ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Idx", ImGuiTableColumnFlags_NoSortDescending);
            ImGui::TableSetupColumn("View name", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Objects");
            ImGui::TableSetupColumn("Meshlets");
            ImGui::TableHeadersRow();

            using rowType = std::tuple<size_t, const char*, uint, uint>;
            rowType rows[BLK_RENDER_VIEW_COUNT];

            // fill rows with data
            for (size_t i = 0; i < BLK_RENDER_VIEW_COUNT; ++i)
            {
                rows[i] = {i, GetViewName(i), debugStats.visiblePerFrustum[i].visibleObjectCount,
                           debugStats.visiblePerFrustum[i].visibleMeshletCount};
            }

            // Sort rows if needed
            const ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
            BLK_ASSERT(sortSpecs->SpecsCount <= 1);

            if (sortSpecs->SpecsCount > 0)
            {
                size_t columnIndex = sortSpecs->Specs[0].ColumnIndex;
                bool reversed = sortSpecs->Specs[0].SortDirection != ImGuiSortDirection_Ascending;

                switch (columnIndex)
                {
                case 0:
                    std::ranges::sort(rows, TupleSorter<0, rowType>{reversed});
                    break;
                case 2:
                    std::ranges::sort(rows, TupleSorter<2, rowType>{reversed});
                    break;
                case 3:
                    std::ranges::sort(rows, TupleSorter<3, rowType>{reversed});
                    break;
                }
            }

            // Output rows
            for (size_t i = 0; i < BLK_RENDER_VIEW_COUNT; ++i)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", std::get<0>(rows[i]));
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(std::get<1>(rows[i]));
                ImGui::TableNextColumn();
                ImGui::Text("%d", std::get<2>(rows[i]));
                ImGui::TableNextColumn();
                ImGui::Text("%d", std::get<3>(rows[i]));
            }

            ImGui::EndTable();
        }
    }

    void DebugOverlayPass::ImguiGPUDebugMarkers(const RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        const auto& debugStats = frameContext.GetFrameStats();

        static const size_t elementsPerLine = 16;

        bool anyFlagsFound = false;

        static bool interpretAsFloat = false;
        ImGui::Checkbox("Interpret values as float", &interpretAsFloat);

        for (size_t i = 0; i < 256; ++i)
        {
            uint markerValue = debugStats.gpuDebugMarkers[i];
            if (markerValue)
            {
                anyFlagsFound = true;
                if (interpretAsFloat)
                {
                    ImGui::Text("Flag %d - data %f", i, asfloat(markerValue));
                }
                else
                {
                    ImGui::Text("Flag %d - data %d", i, markerValue);
                }
            }
        }

        if (!anyFlagsFound)
        {
            ImGui::TextUnformatted("No flags set");
        }
    }

} // namespace Boolka

#endif
