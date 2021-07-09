#include "stdafx.h"

#include "DebugOverlayPass.h"

#ifdef BLK_ENABLE_STATS

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
        const std::lock_guard<std::recursive_mutex> lock(g_imguiMutex);

        ImguiFlipFrame();
        ImguiUIManagement(renderContext);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();
        Texture2D& backbuffer = resourceContainer.GetBackBuffer(frameIndex);
        RenderTargetView& backbufferRTV = resourceContainer.GetBackBufferRTV(frameIndex);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        BLK_GPU_SCOPE(commandList.Get(), "DebugOverlayPass");
        BLK_RENDER_DEBUG_ONLY(resourceTracker.ValidateStates(commandList));

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

        ImGui::Begin("Stats");
        const auto& debugStats = frameContext.GetFrameStats();
        const float fps = 1.0f / debugStats.frameTime;
        const float fpsStable = 1.0f / debugStats.frameTimeStable;
        const auto& cameraPos = frameContext.GetCameraPos();
        const auto& viewMatrix = frameContext.GetViewMatrix();
        const Vector3 viewDir{viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]};
        ImGui::Text("%.1f FPS (avg %.1f FPS)", fps, fpsStable);
        ImGui::Text("%.2f ms (avg %.2f ms)", debugStats.frameTime * 1000.0f,
                    debugStats.frameTimeStable * 1000.0f);
        ImGui::Text("Resolution: %dx%d", engineContext.GetBackbufferWidth(),
                    engineContext.GetBackbufferHeight());
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Pos X:%.2f Y:%.2f Z:%.2f", cameraPos.x(), cameraPos.y(), cameraPos.z());
            ImGui::Text("Dir X:%.2f Y:%.2f Z:%.2f", viewDir.x(), viewDir.y(), viewDir.z());
        }
        if (ImGui::CollapsingHeader("Culling", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImguiCullingTable(renderContext);
        }
        if (ImGui::CollapsingHeader("GPU Debug Markers", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImguiGPUDebugMarkers(renderContext);
        }
        ImGui::End();
        ImGui::Render();
    }

    void DebugOverlayPass::ImguiCullingTable(const RenderContext& renderContext)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        const auto& debugStats = frameContext.GetFrameStats();
        if (ImGui::BeginTable("Visible objects", 4,
                              ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Index");
            ImGui::TableSetupColumn("View name");
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
                    std::ranges::sort(rows, [reversed](const rowType& a, const rowType& b) -> bool {
                        if (reversed)
                        {
                            return std::get<0>(a) < std::get<0>(b);
                        }
                        return std::get<0>(a) > std::get<0>(b);
                    });
                    break;
                case 1:
                    std::ranges::sort(rows, [reversed](const rowType& a, const rowType& b) -> bool {
                        if (reversed)
                        {
                            return strcmp(std::get<1>(a), std::get<1>(b)) > 0;
                        }
                        return strcmp(std::get<1>(a), std::get<1>(b)) < 0;
                    });
                    break;
                case 2:
                    std::ranges::sort(rows, [reversed](const rowType& a, const rowType& b) -> bool {
                        if (reversed)
                        {
                            return std::get<2>(a) > std::get<2>(b);
                        }
                        return std::get<2>(a) < std::get<2>(b);
                    });
                    break;
                case 3:
                    std::ranges::sort(rows, [reversed](const rowType& a, const rowType& b) -> bool {
                        if (reversed)
                        {
                            return std::get<3>(a) > std::get<3>(b);
                        }
                        return std::get<3>(a) < std::get<3>(b);
                    });
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

        for (size_t i = 0; i < 256 / elementsPerLine; ++i)
        {
            for (size_t j = 0; j < elementsPerLine; ++j)
            {
                if (debugStats.gpuDebugMarkers[i * elementsPerLine + j] == 0)
                {
                    ImGui::TextUnformatted("0 ");
                }
                else
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%d ", debugStats.gpuDebugMarkers[i * elementsPerLine + j]);
                }
                ImGui::SameLine();
            }
            ImGui::TextUnformatted("");
        }
    }

} // namespace Boolka

#endif
