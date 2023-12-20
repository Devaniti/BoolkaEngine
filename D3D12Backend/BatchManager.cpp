#include "stdafx.h"

#include "BatchManager.h"

#include "APIWrappers/CommandList/CommandList.h"
#include "BoolkaCommon/Structures/Frustum.h"
#include "Containers/Scene.h"
#include "Contexts/RenderFrameContext.h"

namespace Boolka
{

    BLK_DEFINE_ENUM_OPERATORS(BatchManager::BatchType);
    BLK_DEFINE_ENUM_OPERATORS(BatchManager::ViewType);

    bool BatchManager::Initialize(Device& device, const Scene& scene,
                                  RenderEngineContext& engineContext)
    {
        const UINT opaqueOffset = 0;
        const UINT opaqueCount = scene.GetOpaqueObjectCount();
        const UINT transparentOffset = opaqueCount;
        const UINT trasparentCount = scene.GetObjectCount() - opaqueCount;

        for (BatchType batch = BatchType::Opaque; batch < BatchType::Count; ++batch)
        {
            auto& batchData = m_Batches[static_cast<size_t>(batch)];

            // switch (batch)
            //{
            // case BatchType::Transparent:
            //     batchData.objectCount = trasparentCount;
            //     batchData.objectOffset = transparentOffset;
            //     break;
            // default:
            batchData.objectCount = opaqueCount;
            batchData.objectOffset = opaqueOffset;
            //    break;
            //}
        }

        D3D12_INDIRECT_ARGUMENT_DESC arguments[1]{};

        arguments[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;

        bool result = m_CommandSignature.Initialize(device, nullptr, sizeof(HLSLShared::CullingCommandSignature),
                                                    ARRAYSIZE(arguments), arguments);
        BLK_ASSERT(result);

        return true;
    }

    void BatchManager::Unload()
    {
        m_CommandSignature.Unload();
    }

    bool BatchManager::PrepareBatches(const RenderFrameContext& frameContext, const Scene& scene)
    {
        return true;
    }

    bool BatchManager::Render(CommandList& commandList, RenderContext& renderContext,
                              BatchType batch)
    {
        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        BLK_ASSERT(batch < BatchType::Count);

        const auto& batchData = m_Batches[static_cast<size_t>(batch)];

        auto& resourceContainer = engineContext.GetResourceContainer();
        auto& commandBuffer =
            resourceContainer.GetBuffer(ResourceContainer::Buf::GPUCullingCommand);
        UINT64 viewIndex = static_cast<UINT64>(batch);
        UINT64 commandBufferOffset = viewIndex * sizeof(HLSLShared::CullingCommandSignature);

        commandList->ExecuteIndirect(m_CommandSignature.Get(), 1, commandBuffer.Get(),
                                     commandBufferOffset, nullptr, 0);

        return true;
    }

} // namespace Boolka
