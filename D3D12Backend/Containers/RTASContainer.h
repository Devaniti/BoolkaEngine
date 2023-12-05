#pragma once

#include "APIWrappers/DirectStorage/DStorageFile.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/Resources/Buffers/ReadbackBuffer.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "Streaming/SceneDataReader.h"

namespace Boolka
{

    class Device;
    class RenderEngineContext;

    class [[nodiscard]] RTASContainer
    {
    public:
        bool Initialize(Device& device, RenderEngineContext& engineContext,
                        const SceneDataReader::HeaderWrapper& headerWrapper, Buffer& vertexBuffer,
                        Buffer& indexBuffer);
        void SafeUnload();
        void Unload();

        void FinishLoading(Device& device, RenderEngineContext& engineContext,
                           const SceneDataReader::HeaderWrapper& headerWrapper);
        void FinishInitialization();

    private:
        void PrecalculateAS(Device& device, const SceneDataReader::HeaderWrapper& headerWrapper);
        void BuildAS(GraphicCommandListImpl& initCommandList, Device& device,
                     RenderEngineContext& engineContext,
                     const SceneDataReader::HeaderWrapper& headerWrapper, Buffer& vertexBuffer,
                     Buffer& indexBuffer);
        void CompactAS(GraphicCommandListImpl& initCommandList, Device& device,
                       RenderEngineContext& engineContext,
                       const SceneDataReader::HeaderWrapper& headerWrapper);
#ifdef BLK_ENABLE_RTAS_CACHE
        bool IsRTCacheValid(Device& device, const SceneDataReader::HeaderWrapper& headerWrapper,
                            MemoryBlock& rtCacheheaderWrapper);
        void DeserializeAS(GraphicCommandListImpl& initCommandList, Device& device,
                           RenderEngineContext& engineContext,
                           const SceneDataReader::HeaderWrapper& headerWrapper,
                           MemoryBlock& rtCacheheaderWrapper);
        void SerializeAS(GraphicCommandListImpl& initCommandList, Device& device,
                         RenderEngineContext& engineContext,
                         const SceneDataReader::HeaderWrapper& headerWrapper);

        struct RTCacheHeader
        {
            UINT sceneIdentifier;
            D3D12_SERIALIZED_DATA_DRIVER_MATCHING_IDENTIFIER driverID;
            UINT64 serializedSize;
            UINT64 deserializedSize;
        };

        struct RTCacheObjectData
        {
            UINT64 serializedBLASSize;
            UINT64 deserializedBLASSize;
        };
#endif

        Buffer m_ASBuffer;

        // Initialization Resources
#ifdef BLK_ENABLE_RTAS_CACHE
        bool m_LoadRTASFromCache;
        DStorageFile m_RTASCacheFile;
        Buffer m_RTASSerializedDataBuffer;
        Buffer m_SerializedBLASes;
        ReadbackBuffer m_SerialezedBLASesReadback;
#endif

        Buffer m_ASBuildScratchBuffer;
        Buffer m_BuildBuffer;
        UploadBuffer m_TLASParametersUploadBuffer;
        Buffer m_TLASParametersBuffer;
        Buffer m_PostBuildDataBuffer;
        ReadbackBuffer m_PostBuildDataReadbackBuffer;
        std::vector<UINT64> m_ScratchBufferOffsets;
        std::vector<UINT64> m_BuildOffsets;
        std::vector<UINT64> m_CopyOffsets;
    };

} // namespace Boolka
