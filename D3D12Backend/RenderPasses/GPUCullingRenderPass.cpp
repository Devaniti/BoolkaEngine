#include "stdafx.h"

#include "GPUCullingRenderPass.h"

#include "APIWrappers/Resources/UAVBarrier.h"

namespace Boolka
{

    bool GPUCullingRenderPass::Render(RenderContext& renderContext,
                                      ResourceTracker& resourceTracker)
    {
        BLK_RENDER_PASS_START(GPUCullingRenderPass);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();
        auto& resourceContainer = engineContext.GetResourceContainer();

        UINT frameIndex = frameContext.GetFrameIndex();

        Buffer& frameConstantBuffer = resourceContainer.GetBuffer(ResourceContainer::Buf::Frame);
        Buffer& passConstantBuffer =
            resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCullingCB);
        Buffer& gpuCullingCommmandBuf =
            resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCullingCommand);
        Buffer& gpuCullingMeshletIndicesBuf =
            resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCullingMeshletIndices);
        auto& defaultRootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        resourceTracker.Transition(gpuCullingCommmandBuf, commandList,
                                   D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        resourceTracker.Transition(gpuCullingMeshletIndicesBuf, commandList,
                                   D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        commandList->SetComputeRootSignature(defaultRootSig.Get());
        engineContext.BindSceneResourcesCompute(commandList);
        commandList->SetComputeRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::FrameConstantBuffer),
            frameConstantBuffer->GetGPUVirtualAddress());
        commandList->SetComputeRootConstantBufferView(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassConstantBuffer),
            passConstantBuffer->GetGPUVirtualAddress());

        const LightContainer& lightContainer = frameContext.GetLightContainer();
        UINT viewCount =
            static_cast<UINT>(BatchManager::ViewType::ShadowMapLight0) +
            BLK_TEXCUBE_FACE_COUNT * static_cast<UINT>(lightContainer.GetLights().size());
        UINT objectCount = engineContext.GetScene().GetOpaqueObjectCount();

        const UINT clearValues[4] = {};

        commandList->SetPipelineState(
            engineContext.GetPSOContainer().GetPSO(PSOContainer::ComputePSO::GPUCulling).Get());
        commandList->Dispatch(viewCount, 1, 1);

        resourceTracker.Transition(gpuCullingCommmandBuf, commandList,
                                   D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        resourceTracker.Transition(gpuCullingMeshletIndicesBuf, commandList,
                                   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        return true;
    }

    bool GPUCullingRenderPass::PrepareRendering()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    bool GPUCullingRenderPass::Initialize(Device& device, RenderContext& renderContext)
    {
#ifdef BLK_ENABLE_STATS
        for (size_t i = 0; i < BLK_IN_FLIGHT_FRAMES; i++)
            m_CulledCountBuffer[i].Initialize(device, sizeof(FrameStats::visiblePerFrustum));
#endif

        return true;
    }

    void GPUCullingRenderPass::Unload()
    {
#ifdef BLK_ENABLE_STATS
        BLK_UNLOAD_ARRAY(m_CulledCountBuffer);
#endif
    }

} // namespace Boolka
