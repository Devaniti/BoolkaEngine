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
        Buffer& gpuCullingUAVBuf = resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCulling);
        Buffer& gpuCullingCommmandBuf =
            resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCullingCommand);
        Buffer& gpuCullingMeshletIndiciesBuf =
            resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCullingMeshletIndices);
        D3D12_GPU_DESCRIPTOR_HANDLE gpuCullingUAVBufGPUDescriptor =
            resourceContainer.GetGPUDescriptor(ResourceContainer::Buf::GPUCulling);
        D3D12_CPU_DESCRIPTOR_HANDLE gpuCullingUAVBufCPUDescriptor =
            resourceContainer.GetCPUVisibleCPUDescriptor(ResourceContainer::Buf::GPUCulling);
        D3D12_GPU_DESCRIPTOR_HANDLE gpuCullingCommandUINTUAVBufGPUDescriptor =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap)
                .GetGPUHandle(static_cast<size_t>(
                                  ResourceContainer::MainSRVDescriptorHeapOffsets::UAVHeapOffset) +
                              static_cast<size_t>(ResourceContainer::UAV::GPUCullingCommandUINT));
        D3D12_CPU_DESCRIPTOR_HANDLE gpuCullingCommandUAVBufCPUDescriptor =
            resourceContainer.GetCPUVisibleCPUDescriptor(ResourceContainer::Buf::GPUCullingCommand);
        auto& defaultRootSig =
            resourceContainer.GetRootSignature(ResourceContainer::RootSig::Default);

        GraphicCommandListImpl& commandList = threadContext.GetGraphicCommandList();

        resourceTracker.Transition(gpuCullingCommmandBuf, commandList,
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

        UAVBarrier::Barrier(commandList, gpuCullingUAVBuf);
        commandList->ClearUnorderedAccessViewUint(gpuCullingUAVBufGPUDescriptor,
                                                  gpuCullingUAVBufCPUDescriptor,
                                                  gpuCullingUAVBuf.Get(), clearValues, 0, nullptr);
        commandList->ClearUnorderedAccessViewUint(
            gpuCullingCommandUINTUAVBufGPUDescriptor, gpuCullingCommandUAVBufCPUDescriptor,
            gpuCullingCommmandBuf.Get(), clearValues, 0, nullptr);

        UAVBarrier::Barrier(commandList, gpuCullingUAVBuf);
        commandList->SetPipelineState(
            engineContext.GetPSOContainer().GetPSO(PSOContainer::ComputePSO::ObjectCulling).Get());
        commandList->Dispatch(BLK_INT_DIVIDE_CEIL(objectCount, 32), viewCount, 1);

        UAVBarrier::Barrier(commandList, gpuCullingUAVBuf);
        UAVBarrier::Barrier(commandList, gpuCullingCommmandBuf);
        commandList->SetPipelineState(
            engineContext.GetPSOContainer()
                .GetPSO(PSOContainer::ComputePSO::CullingCommandBufferGeneration)
                .Get());
        commandList->Dispatch(BLK_INT_DIVIDE_CEIL(objectCount, 32), viewCount, 1);

        resourceTracker.Transition(gpuCullingCommmandBuf, commandList,
                                   D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);

#ifdef BLK_ENABLE_STATS

        Buffer& gpuCullingReadbackUAVBuf =
            resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCullingDebugReadback);

        UAVBarrier::Barrier(commandList, gpuCullingUAVBuf);

        commandList->SetPipelineState(engineContext.GetPSOContainer()
                                          .GetPSO(PSOContainer::ComputePSO::CullingDebugReadback)
                                          .Get());
        commandList->Dispatch(viewCount, 1, 1);
        auto& debugStats = frameContext.GetFrameStats();

        m_CulledCountBuffer[frameIndex].Readback(debugStats.visiblePerFrustum,
                                                 sizeof(debugStats.visiblePerFrustum));

        resourceTracker.Transition(gpuCullingReadbackUAVBuf, commandList,
                                   D3D12_RESOURCE_STATE_COPY_SOURCE);
        commandList->CopyBufferRegion(m_CulledCountBuffer[frameIndex].Get(), 0,
                                      gpuCullingReadbackUAVBuf.Get(), 0,
                                      sizeof(FrameStats::visiblePerFrustum));
        resourceTracker.Transition(gpuCullingReadbackUAVBuf, commandList,
                                   D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
#endif

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
