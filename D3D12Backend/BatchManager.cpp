#include "stdafx.h"

#include "BatchManager.h"

#include "APIWrappers/CommandList/CommandList.h"
#include "BoolkaCommon/Structures/Frustum.h"
#include "Containers/Scene.h"
#include "Contexts/RenderFrameContext.h"

namespace Boolka
{

    BLK_DEFINE_ENUM_OPERATORS(BatchManager::BatchType);

    BatchManager::BatchManager()
    {
    };

    bool BatchManager::Initialize(const Scene& scene)
    {
        const UINT opaqueOffset = 0;
        const UINT opaqueCount = scene.GetOpaqueObjectCount();
        const UINT transparentOffset = opaqueCount;
        const UINT trasparentCount = scene.GetObjectCount() - opaqueCount;

        for (BatchType batch = BatchType::Opaque; batch < BatchType::Count; ++batch)
        {
            auto& batchData = m_batches[static_cast<size_t>(batch)];

            switch (batch)
            {
            case BatchType::Transparent:
                batchData.objectCount = trasparentCount;
                batchData.objectOffset = transparentOffset;
                break;
            default:
                batchData.objectCount = opaqueCount;
                batchData.objectOffset = opaqueOffset;
                break;
            }
        }

        return true;
    }

    void BatchManager::Unload()
    {
    }

    bool BatchManager::PrepareBatches(const RenderFrameContext& frameContext, const Scene& scene)
    {
        return true;
    }

    bool BatchManager::Render(CommandList& commandList, BatchType batch) const
    {
        BLK_ASSERT(batch < BatchType::Count);
        const auto& batchData = m_batches[static_cast<size_t>(batch)];
        commandList->SetGraphicsRoot32BitConstant(
            static_cast<UINT>(ResourceContainer::DefaultRootSigBindPoints::PassRootConstant),
            batchData.objectOffset, 0);
        commandList->DispatchMesh(batchData.objectCount, 1, 1);
        return true;
    }

} // namespace Boolka
