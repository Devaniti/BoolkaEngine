#include "stdafx.h"

#include "RTASContainer.h"

#include "APIWrappers/Raytracing/AccelerationStructure/BottomLevelAS.h"
#include "APIWrappers/Raytracing/AccelerationStructure/TopLevelAS.h"
#include "BoolkaCommon/DebugHelpers/DebugFileWriter.h"
#include "Scene.h"

namespace Boolka
{

    bool RTASContainer::Initialize(Device& device, RenderEngineContext& engineContext,
                                   const SceneDataReader::HeaderWrapper& headerWrapper,
                                   Buffer& vertexBuffer, Buffer& indexBuffer)
    {
        BLK_CPU_SCOPE("RTASContainer::Initialize");

        m_ScratchBufferOffsets.resize(headerWrapper.header->opaqueCount);
        m_BuildOffsets.resize(headerWrapper.header->opaqueCount);
        m_CopyOffsets.resize(headerWrapper.header->opaqueCount);

#ifdef BLK_ENABLE_RTAS_CACHE
        MemoryBlock rtCacheheaderWrapper{};
        m_LoadRTASFromCache = IsRTCacheValid(device, headerWrapper, rtCacheheaderWrapper);

        g_WDebugOutput << (m_LoadRTASFromCache ? L"Loading BLASes from cache" : L"Building BLASes")
                       << std::endl;

        if (m_LoadRTASFromCache)
        {
            auto& initCommandList = engineContext.GetInitializationCommandList();
            DeserializeAS(initCommandList, device, engineContext, headerWrapper,
                          rtCacheheaderWrapper);
            engineContext.ExecuteInitializationCommandList(device);
            engineContext.ResetInitializationCommandList();
        }
        else
#endif
        {
            PrecalculateAS(device, headerWrapper);
            device.GetDStorageQueue().SyncGPU(device.GetGraphicQueue());
            auto& initCommandList = engineContext.GetInitializationCommandList();
            BuildAS(initCommandList, device, engineContext, headerWrapper, vertexBuffer,
                    indexBuffer);
            engineContext.ExecuteInitializationCommandList(device);
            engineContext.ResetInitializationCommandList();
        }

        return true;
    }

    void RTASContainer::SafeUnload()
    {
        m_ASBuffer.SafeUnload();
    }

    void RTASContainer::Unload()
    {
        m_ASBuffer.Unload();
    }

    void RTASContainer::FinishLoading(Device& device, RenderEngineContext& engineContext,
                                      const SceneDataReader::HeaderWrapper& headerWrapper)
    {
        BLK_CPU_SCOPE("RTASContainer::FinishLoading");
#ifdef BLK_ENABLE_RTAS_CACHE
        if (!m_LoadRTASFromCache)
#endif
        {
            device.GetGraphicQueue().Flush();
            auto& initCommandList = engineContext.GetInitializationCommandList();
            CompactAS(initCommandList, device, engineContext, headerWrapper);
#ifdef BLK_ENABLE_RTAS_CACHE
            SerializeAS(initCommandList, device, engineContext, headerWrapper);
#endif
        }
    }

    void RTASContainer::FinishInitialization()
    {
#ifdef BLK_ENABLE_RTAS_CACHE
        if (m_LoadRTASFromCache)
        {
            m_ASBuildScratchBuffer.Unload();
            m_RTASSerializedDataBuffer.Unload();
            m_TLASParametersUploadBuffer.Unload();
            m_TLASParametersBuffer.Unload();
            m_RTASCacheFile.CloseFile();
        }
        else
#endif
        {
            m_ASBuildScratchBuffer.Unload();
            m_BuildBuffer.Unload();
            m_TLASParametersUploadBuffer.Unload();
            m_TLASParametersBuffer.Unload();
            m_PostBuildDataBuffer.Unload();
            m_PostBuildDataReadbackBuffer.Unload();
#ifdef BLK_ENABLE_RTAS_CACHE
            m_SerializedBLASes.Unload();
            m_SerialezedBLASesReadback.Unload();
#endif
        }
    }

    void RTASContainer::PrecalculateAS(Device& device,
                                       const SceneDataReader::HeaderWrapper& headerWrapper)
    {
        const auto* objects = headerWrapper.cpuObjectHeaders;
        const auto& dataHeader = *headerWrapper.header;
        const UINT objectCount = dataHeader.opaqueCount;

        UINT64 scratchSize = 0;
        UINT64 asSize = 0;
        for (size_t i = 0; i < objectCount; ++i)
        {
            UINT vertexSize = 16;
            UINT64 currentScratchSize = 0;
            UINT64 currentASSize = 0;
            BottomLevelAS::GetSizes(device, dataHeader.vertex1Size / vertexSize, vertexSize,
                                    objects[i].rtIndexCount, currentScratchSize, currentASSize);
            m_ScratchBufferOffsets[i] = scratchSize;
            m_BuildOffsets[i] = asSize;
            scratchSize += currentScratchSize;
            asSize += currentASSize;
        }

        UINT64 tlasScratchSize;
        UINT64 tlasSize;
        TopLevelAS::GetSizes(device, objectCount, tlasScratchSize, tlasSize);
        scratchSize = std::max(scratchSize, tlasScratchSize);

        UINT64 tlasParametersSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * objectCount;

        m_TLASParametersUploadBuffer.Initialize(device, tlasParametersSize);
        m_TLASParametersBuffer.Initialize(device, tlasParametersSize, D3D12_HEAP_TYPE_DEFAULT,
                                          D3D12_RESOURCE_FLAG_NONE);
        m_ASBuildScratchBuffer.Initialize(device, scratchSize, D3D12_HEAP_TYPE_DEFAULT,
                                          D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        m_BuildBuffer.Initialize(device, asSize, D3D12_HEAP_TYPE_DEFAULT,
                                 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                                 D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

        RenderDebug::SetDebugName(m_TLASParametersUploadBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_TLASParametersUploadBuffer");
        RenderDebug::SetDebugName(m_TLASParametersBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_TLASParametersBuffer");
        RenderDebug::SetDebugName(m_ASBuildScratchBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_ASBuildScratchBuffer");
        RenderDebug::SetDebugName(m_BuildBuffer.Get(), L"%ls", L"RTASContainer::m_BuildBuffer");

        g_WDebugOutput << "Pre-Compaction BLAS total size: " << asSize / 1024.0f / 1024.0f << "MB"
                       << std::endl;

#ifdef BLK_ENABLE_RTAS_CACHE
        static const size_t postBuildInfoElementSize = std::max(
            sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC),
            sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_SERIALIZATION_DESC));
#else
        static const size_t postBuildInfoElementSize =
            sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC);
#endif

        static const size_t postBuildInfoSize = postBuildInfoElementSize * objectCount;
        m_PostBuildDataBuffer.Initialize(device, postBuildInfoSize, D3D12_HEAP_TYPE_DEFAULT,
                                         D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        m_PostBuildDataReadbackBuffer.Initialize(device, postBuildInfoSize);
        RenderDebug::SetDebugName(m_PostBuildDataBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_PostBuildDataBuffer");
        RenderDebug::SetDebugName(m_PostBuildDataReadbackBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_PostBuildDataReadbackBuffer");
    }

    void RTASContainer::BuildAS(GraphicCommandListImpl& initCommandList, Device& device,
                                RenderEngineContext& engineContext,
                                const SceneDataReader::HeaderWrapper& headerWrapper,
                                Buffer& vertexBuffer, Buffer& indexBuffer)
    {
        BLK_CPU_SCOPE("RTASContainer::BuildAS");

        const auto& dataHeader = *headerWrapper.header;
        const UINT objectCount = dataHeader.opaqueCount;
        const auto* objects = headerWrapper.cpuObjectHeaders;
        UINT64 buildBufferAddress = m_BuildBuffer->GetGPUVirtualAddress();
        UINT64 scratchBufferAddress = m_ASBuildScratchBuffer->GetGPUVirtualAddress();
        UINT64 vertexBufferAddress = vertexBuffer->GetGPUVirtualAddress();
        UINT vertexSize = 16;
        UINT64 indexBufferAddress = indexBuffer->GetGPUVirtualAddress();
        UINT64 postBuildDataAddress = m_PostBuildDataBuffer->GetGPUVirtualAddress();
        {
            BLK_GPU_SCOPE(initCommandList, "Scene::BuildAS");
            {
                for (size_t i = 0; i < objectCount; ++i)
                {
                    UINT64 postBuildDataOffset =
                        sizeof(
                            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC) *
                        i;
                    BottomLevelAS::Initialize(
                        initCommandList, buildBufferAddress + m_BuildOffsets[i],
                        scratchBufferAddress + m_ScratchBufferOffsets[i], vertexBufferAddress,
                        dataHeader.vertex1Size / vertexSize, vertexSize,
                        indexBufferAddress + objects[i].rtIndexOffset * sizeof(uint32_t),
                        objects[i].rtIndexCount, postBuildDataAddress + postBuildDataOffset);
                }
            }

            ResourceTransition::Transition(initCommandList, m_PostBuildDataBuffer,
                                           D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                           D3D12_RESOURCE_STATE_COPY_SOURCE);
            size_t postBuildInfoSize =
                sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC) *
                objectCount;
            initCommandList->CopyResource(m_PostBuildDataReadbackBuffer.Get(),
                                          m_PostBuildDataBuffer.Get());
        }
    }

    void RTASContainer::CompactAS(GraphicCommandListImpl& initCommandList, Device& device,
                                  RenderEngineContext& engineContext,
                                  const SceneDataReader::HeaderWrapper& headerWrapper)
    {
        BLK_CPU_SCOPE("RTASContainer::CompactAS");

        BLK_GPU_SCOPE(initCommandList, "Scene::CompactAS");
        const auto& dataHeader = *headerWrapper.header;
        const UINT objectCount = dataHeader.opaqueCount;
        const auto* objects = headerWrapper.cpuObjectHeaders;

        ResourceContainer& resourceContainer = engineContext.GetResourceContainer();
        DescriptorHeap& mainSRVHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        UINT mainSRVHeapOffset =
            static_cast<UINT>(ResourceContainer::MainSRVDescriptorHeapOffsets::SceneSRVHeapOffset);

        UINT64 buildBufferAddress = m_BuildBuffer->GetGPUVirtualAddress();
        UINT64 scratchBufferAddress = m_ASBuildScratchBuffer->GetGPUVirtualAddress();

        void* postBuildData = m_PostBuildDataReadbackBuffer.Map(
            0, sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC) *
                   objectCount);
        auto* postBuildDescs = static_cast<
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_COMPACTED_SIZE_DESC*>(
            postBuildData);

        UINT64 asFinalSize = 0;

        for (size_t i = 0; i < objectCount; ++i)
        {
            asFinalSize +=
                BLK_CEIL_TO_POWER_OF_TWO(postBuildDescs[i].CompactedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        }

        g_WDebugOutput << "Post-Compaction BLAS total size: " << asFinalSize / 1024.0f / 1024.0f
                       << "MB" << std::endl;

        UINT64 scratchSize;
        UINT64 tlasSize;
        TopLevelAS::GetSizes(device, objectCount, scratchSize, tlasSize);
        asFinalSize += tlasSize;

        g_WDebugOutput << "TLAS size: " << tlasSize / 1024.0f / 1024.0f << "MB" << std::endl;

        m_ASBuffer.Initialize(device, asFinalSize, D3D12_HEAP_TYPE_DEFAULT,
                              D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                              D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
        RenderDebug::SetDebugName(m_ASBuffer.Get(), L"RTASContainer::m_ASBuffer");

        UINT64 asDestAddress = m_ASBuffer->GetGPUVirtualAddress();
        UINT64 blasDestAddress = asDestAddress + tlasSize;

        UINT64 currentBlasDestAddress = blasDestAddress;
        {
            BLK_GPU_SCOPE(initCommandList, "CopyAndCompactBLAS");
            for (size_t i = 0; i < objectCount; ++i)
            {
                initCommandList->CopyRaytracingAccelerationStructure(
                    currentBlasDestAddress, buildBufferAddress + m_BuildOffsets[i],
                    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_COMPACT);
                m_CopyOffsets[i] = currentBlasDestAddress;
                currentBlasDestAddress += BLK_CEIL_TO_POWER_OF_TWO(
                    postBuildDescs[i].CompactedSizeInBytes,
                    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
            }
        }

        currentBlasDestAddress = blasDestAddress;

        {
            void* tlasParametersData = m_TLASParametersUploadBuffer.Map();
            auto* tlasParameters = static_cast<D3D12_RAYTRACING_INSTANCE_DESC*>(tlasParametersData);

            for (size_t i = 0; i < objectCount; ++i)
            {
                D3D12_RAYTRACING_INSTANCE_DESC& param = tlasParameters[i];
                // Identity transform matrix
                param = {};
                param.Transform[0][0] = 1.0f;
                param.Transform[1][1] = 1.0f;
                param.Transform[2][2] = 1.0f;

                param.InstanceID = objects[i].materialIndex; // Material index
                param.InstanceMask = 1;
                param.InstanceContributionToHitGroupIndex = 0;
                param.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
                param.AccelerationStructure = currentBlasDestAddress;
                currentBlasDestAddress += BLK_CEIL_TO_POWER_OF_TWO(
                    postBuildDescs[i].CompactedSizeInBytes,
                    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
            }
            m_TLASParametersUploadBuffer.Unmap();
        }

        m_PostBuildDataReadbackBuffer.Unmap();

        initCommandList->CopyResource(m_TLASParametersBuffer.Get(),
                                      m_TLASParametersUploadBuffer.Get());
        ResourceTransition::Transition(initCommandList, m_TLASParametersBuffer,
                                       D3D12_RESOURCE_STATE_COPY_DEST,
                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

        UINT64 tlasParametersAddress = m_TLASParametersBuffer->GetGPUVirtualAddress();

        TopLevelAS::Initialize(initCommandList, asDestAddress, scratchBufferAddress,
                               tlasParametersAddress, objectCount);

        ShaderResourceView::InitializeAccelerationStructure(
            device, asDestAddress,
            mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + Scene::SRVOffset::RaytracingASOffset));
    }

#ifdef BLK_ENABLE_RTAS_CACHE
    bool RTASContainer::IsRTCacheValid(Device& device,
                                       const SceneDataReader::HeaderWrapper& headerWrapper,
                                       MemoryBlock& rtCacheheaderWrapper)
    {
        BLK_CPU_SCOPE("RTASContainer::IsRTCacheValid");

        const auto& dataHeader = *headerWrapper.header;

        rtCacheheaderWrapper = DebugFileReader::ReadFile(BLK_CACHE_RT_HEADER_FILENAME);

        if (rtCacheheaderWrapper.m_Data == nullptr)
            return false;

        const RTCacheHeader* rtCacheHeader =
            ptr_static_cast<const RTCacheHeader*>(rtCacheheaderWrapper.m_Data);
        if (rtCacheHeader->sceneIdentifier != dataHeader.sceneIdentifier)
            return false;
        D3D12_DRIVER_MATCHING_IDENTIFIER_STATUS driverStatus =
            device->CheckDriverMatchingIdentifier(
                D3D12_SERIALIZED_DATA_RAYTRACING_ACCELERATION_STRUCTURE, &rtCacheHeader->driverID);
        if (driverStatus != D3D12_DRIVER_MATCHING_IDENTIFIER_COMPATIBLE_WITH_DEVICE)
        {
            DebugFileReader::FreeMemory(rtCacheheaderWrapper);
            return false;
        }

        return true;
    }

    void RTASContainer::DeserializeAS(GraphicCommandListImpl& initCommandList, Device& device,
                                      RenderEngineContext& engineContext,
                                      const SceneDataReader::HeaderWrapper& headerWrapper,
                                      MemoryBlock& rtCacheheaderWrapper)
    {
        BLK_CPU_SCOPE("RTASContainer::DeserializeAS");

        const auto& dataHeader = *headerWrapper.header;
        const UINT objectCount = dataHeader.opaqueCount;
        const auto* objects = headerWrapper.cpuObjectHeaders;

        ResourceContainer& resourceContainer = engineContext.GetResourceContainer();
        DescriptorHeap& mainSRVHeap =
            resourceContainer.GetDescriptorHeap(ResourceContainer::DescHeap::MainHeap);
        UINT mainSRVHeapOffset =
            static_cast<UINT>(ResourceContainer::MainSRVDescriptorHeapOffsets::SceneSRVHeapOffset);

        m_RTASCacheFile.OpenFile(device.GetDStorageFactory(), BLK_CACHE_RT_FILENAME);

        const RTCacheHeader* rtCacheHeader =
            static_cast<const RTCacheHeader*>(rtCacheheaderWrapper.m_Data);
        const RTCacheObjectData* objectSerializedData = static_cast<const RTCacheObjectData*>(
            OffsetPtr(rtCacheheaderWrapper.m_Data, sizeof(RTCacheHeader)));

        UINT64 asTotalSize = rtCacheHeader->deserializedSize;

        UINT64 scratchSize = 0;
        UINT64 tlasSize = 0;

        TopLevelAS::GetSizes(device, objectCount, scratchSize, tlasSize);

        m_ASBuffer.Initialize(device, rtCacheHeader->deserializedSize + tlasSize,
                              D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                              D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
        m_ASBuildScratchBuffer.Initialize(device, scratchSize, D3D12_HEAP_TYPE_DEFAULT,
                                          D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        m_RTASSerializedDataBuffer.Initialize(device, rtCacheHeader->serializedSize,
                                              D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE);

        RenderDebug::SetDebugName(m_ASBuffer.Get(), L"%ls", L"RTASContainer::m_ASBuffer");
        RenderDebug::SetDebugName(m_ASBuildScratchBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_ASBuildScratchBuffer");
        RenderDebug::SetDebugName(m_RTASSerializedDataBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_RTASSerializedDataBuffer");

        device.GetDStorageQueue().EnququeRead(m_RTASCacheFile, 0, rtCacheHeader->serializedSize,
                                              m_RTASSerializedDataBuffer, 0);

        UINT64 serializedDataRead = device.GetDStorageQueue().SignalDStorage();
        device.GetDStorageQueue().SubmitCommands();
        device.GetDStorageQueue().GetFence().WaitGPU(serializedDataRead, device.GetGraphicQueue());

        UINT64 currentSerializedData = m_RTASSerializedDataBuffer->GetGPUVirtualAddress();
        UINT64 currentDeserializedData = m_ASBuffer->GetGPUVirtualAddress() + tlasSize;

        BLK_GPU_SCOPE(initCommandList, "Scene::DeserializeAS");
        {
            for (size_t i = 0; i < objectCount; ++i)
            {
                initCommandList->CopyRaytracingAccelerationStructure(
                    currentDeserializedData, currentSerializedData,
                    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_DESERIALIZE);
                currentSerializedData += objectSerializedData[i].serializedBLASSize;
                currentDeserializedData += objectSerializedData[i].deserializedBLASSize;
            }
        }

        currentDeserializedData = m_ASBuffer->GetGPUVirtualAddress() + tlasSize;

        UINT64 tlasParametersSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * objectCount;

        m_TLASParametersUploadBuffer.Initialize(device, tlasParametersSize);
        m_TLASParametersBuffer.Initialize(device, tlasParametersSize, D3D12_HEAP_TYPE_DEFAULT,
                                          D3D12_RESOURCE_FLAG_NONE);

        RenderDebug::SetDebugName(m_TLASParametersUploadBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_TLASParametersUploadBuffer");
        RenderDebug::SetDebugName(m_TLASParametersBuffer.Get(), L"%ls",
                                  L"RTASContainer::m_TLASParametersBuffer");

        void* tlasParametersData = m_TLASParametersUploadBuffer.Map();
        auto* tlasParameters = static_cast<D3D12_RAYTRACING_INSTANCE_DESC*>(tlasParametersData);

        for (size_t i = 0; i < objectCount; ++i)
        {
            D3D12_RAYTRACING_INSTANCE_DESC& param = tlasParameters[i];
            // Identity transform matrix
            param = {};
            param.Transform[0][0] = 1.0f;
            param.Transform[1][1] = 1.0f;
            param.Transform[2][2] = 1.0f;

            param.InstanceID = objects[i].materialIndex; // Material index
            param.InstanceMask = 1;
            param.InstanceContributionToHitGroupIndex = 0;
            param.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
            param.AccelerationStructure = currentDeserializedData;
            currentDeserializedData += objectSerializedData[i].deserializedBLASSize;
        }
        m_TLASParametersUploadBuffer.Unmap();

        initCommandList->CopyResource(m_TLASParametersBuffer.Get(),
                                      m_TLASParametersUploadBuffer.Get());
        ResourceTransition::Transition(initCommandList, m_TLASParametersBuffer,
                                       D3D12_RESOURCE_STATE_COPY_DEST,
                                       D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

        TopLevelAS::Initialize(initCommandList, m_ASBuffer->GetGPUVirtualAddress(),
                               m_ASBuildScratchBuffer->GetGPUVirtualAddress(),
                               m_TLASParametersBuffer->GetGPUVirtualAddress(), objectCount);

        ShaderResourceView::InitializeAccelerationStructure(
            device, m_ASBuffer->GetGPUVirtualAddress(),
            mainSRVHeap.GetCPUHandle(mainSRVHeapOffset + Scene::SRVOffset::RaytracingASOffset));

        DebugFileReader::FreeMemory(rtCacheheaderWrapper);
    }

    void RTASContainer::SerializeAS(GraphicCommandListImpl& initCommandList, Device& device,
                                    RenderEngineContext& engineContext,
                                    const SceneDataReader::HeaderWrapper& headerWrapper)
    {
        BLK_CPU_SCOPE("RTASContainer::SerializeAS");

        const auto& dataHeader = *headerWrapper.header;
        const UINT objectCount = dataHeader.opaqueCount;

        UINT64 postBuildInfoAddress = m_PostBuildDataBuffer->GetGPUVirtualAddress();

        ResourceTransition::Transition(initCommandList, m_PostBuildDataBuffer,
                                       D3D12_RESOURCE_STATE_COPY_SOURCE,
                                       D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_DESC postBuildInfoDesc = {};
        postBuildInfoDesc.DestBuffer = postBuildInfoAddress;
        postBuildInfoDesc.InfoType =
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_SERIALIZATION;
        initCommandList->EmitRaytracingAccelerationStructurePostbuildInfo(
            &postBuildInfoDesc, objectCount, m_CopyOffsets.data());

        ResourceTransition::Transition(initCommandList, m_PostBuildDataBuffer,
                                       D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                       D3D12_RESOURCE_STATE_COPY_SOURCE);

        initCommandList->CopyBufferRegion(
            m_PostBuildDataReadbackBuffer.Get(), 0, m_PostBuildDataBuffer.Get(), 0,
            sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_SERIALIZATION_DESC) *
                objectCount);

        engineContext.FlushInitializationCommandList(device);

        void* postBuildData = m_PostBuildDataReadbackBuffer.Map(
            0, sizeof(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_SERIALIZATION_DESC) *
                   objectCount);
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_SERIALIZATION_DESC*
            serializationInfo = ptr_static_cast<
                D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_SERIALIZATION_DESC*>(
                postBuildData);
        UINT64 serializedSize = 0;
        for (UINT i = 0; i < objectCount; ++i)
        {
            serializedSize +=
                BLK_CEIL_TO_POWER_OF_TWO(serializationInfo[i].SerializedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
            BLK_ASSERT(serializationInfo[i].NumBottomLevelAccelerationStructurePointers == 0);
        }

        m_SerializedBLASes.Initialize(device, serializedSize, D3D12_HEAP_TYPE_DEFAULT,
                                      D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        m_SerialezedBLASesReadback.Initialize(device, serializedSize);

        RenderDebug::SetDebugName(m_SerializedBLASes.Get(), L"%ls",
                                  L"RTASContainer::m_SerializedBLASes");
        RenderDebug::SetDebugName(m_SerialezedBLASesReadback.Get(), L"%ls",
                                  L"RTASContainer::m_SerialezedBLASesReadback");

        UINT64 serializedDest = m_SerializedBLASes->GetGPUVirtualAddress();

        for (UINT i = 0; i < objectCount; ++i)
        {
            initCommandList->CopyRaytracingAccelerationStructure(
                serializedDest, m_CopyOffsets[i],
                D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE_SERIALIZE);
            serializedDest +=
                BLK_CEIL_TO_POWER_OF_TWO(serializationInfo[i].SerializedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        }

        m_PostBuildDataReadbackBuffer.Unmap();

        ResourceTransition::Transition(initCommandList, m_SerializedBLASes,
                                       D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                       D3D12_RESOURCE_STATE_COPY_SOURCE);

        initCommandList->CopyResource(m_SerialezedBLASesReadback.Get(), m_SerializedBLASes.Get());

        engineContext.FlushInitializationCommandList(device);

        MemoryBlock serializedData{m_SerialezedBLASesReadback.Map(0, serializedSize),
                                   serializedSize};

        UINT64 deserializedSize = 0;
        const char* currentSerializedHeader = static_cast<const char*>(serializedData.m_Data);

        for (size_t i = 0; i < objectCount; ++i)
        {
            auto* currentSerializedASHeader =
                ptr_static_cast<const D3D12_SERIALIZED_RAYTRACING_ACCELERATION_STRUCTURE_HEADER*>(
                    currentSerializedHeader);
            currentSerializedHeader +=
                BLK_CEIL_TO_POWER_OF_TWO(serializationInfo[i].SerializedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
            deserializedSize +=
                BLK_CEIL_TO_POWER_OF_TWO(currentSerializedASHeader->DeserializedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        }

        auto* firstSerializedASHeader =
            ptr_static_cast<const D3D12_SERIALIZED_RAYTRACING_ACCELERATION_STRUCTURE_HEADER*>(
                serializedData.m_Data);

        RTCacheHeader serializedDataHeader{dataHeader.sceneIdentifier,
                                           firstSerializedASHeader->DriverMatchingIdentifier,
                                           serializedSize, deserializedSize};

        DebugFileWriter headerWriter;
        headerWriter.OpenFile(BLK_CACHE_RT_HEADER_FILENAME);

        currentSerializedHeader = static_cast<const char*>(serializedData.m_Data);

        headerWriter.Write(&serializedDataHeader, sizeof(serializedDataHeader));
        for (size_t i = 0; i < objectCount; ++i)
        {
            auto* currentSerializedASHeader =
                ptr_static_cast<const D3D12_SERIALIZED_RAYTRACING_ACCELERATION_STRUCTURE_HEADER*>(
                    currentSerializedHeader);
            currentSerializedHeader +=
                BLK_CEIL_TO_POWER_OF_TWO(serializationInfo[i].SerializedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
            RTCacheObjectData currentObject{
                BLK_CEIL_TO_POWER_OF_TWO(serializationInfo[i].SerializedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT),
                BLK_CEIL_TO_POWER_OF_TWO(currentSerializedASHeader->DeserializedSizeInBytes,
                                         D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT)};
            headerWriter.Write(&currentObject, sizeof(currentObject));
        }

        headerWriter.Close();

        DebugFileWriter::WriteFile(BLK_CACHE_RT_FILENAME, serializedData);

        m_SerialezedBLASesReadback.Unmap();
    }

#endif

} // namespace Boolka
