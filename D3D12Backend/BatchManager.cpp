#include "stdafx.h"

#include "BatchManager.h"

#include "APIWrappers/CommandList/CommandList.h"
#include "BoolkaCommon/Structures/Frustum.h"
#include "Containers/Scene.h"
#include "Contexts/RenderFrameContext.h"

namespace Boolka
{

    BLK_DEFINE_ENUM_OPERATORS(BatchManager::BatchType);

    BatchManager::~BatchManager()
    {
#ifdef BLK_DEBUG
        for (const auto& batch : m_batches)
        {
            BLK_ASSERT(batch.empty());
        }
#endif
    }

    bool BatchManager::Initialize(const Scene& scene)
    {
        GetBatch(BatchType::Opaque).reserve(scene.GetOpaqueObjectCount());
        GetBatch(BatchType::Transparent)
            .reserve(scene.GetObjectCount() - scene.GetOpaqueObjectCount());
        GetBatch(BatchType::ShadowMapSun).reserve(scene.GetOpaqueObjectCount());
        for (size_t i = 0; i < BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT; ++i)
        {
            GetBatch(BatchType::ShadowMapLight0 + i).reserve(32);
        }
        return true;
    }

    void BatchManager::Unload()
    {
        for (auto& batch : m_batches)
        {
            batch.clear();
        }
    }

    bool BatchManager::PrepareBatches(const RenderFrameContext& frameContext, const Scene& scene)
    {
        for (BatchType batch = BatchType::Opaque; batch < BatchType::Count; ++batch)
        {
            if (!IsBatchEnabled(batch, frameContext))
                continue;

            UINT startIndex, endIndex;
            GetBatchRange(batch, scene, startIndex, endIndex);

            Vector3 cameraCoord;
            Matrix4x4 viewProjMatrix;
            GetBatchView(batch, frameContext, cameraCoord, viewProjMatrix);

            std::vector<SortingData> culledObjects;
            CullObjects(scene, viewProjMatrix, cameraCoord, startIndex, endIndex, culledObjects);

            SortObjects(batch, culledObjects);

            GenerateDrawData(batch, scene, culledObjects);
        }

        return true;
    }

    bool BatchManager::Render(CommandList& commandList, BatchType batch)
    {
        BLK_ASSERT(batch < BatchType::Count);
        for (const auto& object : m_batches[static_cast<size_t>(batch)])
        {
            commandList->DrawIndexedInstanced(object.indexCount, 1, object.startIndex, 0, 0);
        }
        return true;
    }

    bool BatchManager::IsBatchEnabled(BatchType batch, const RenderFrameContext& frameContext)
    {
        BLK_ASSERT(batch < BatchType::Count);
        const auto& lightContainer = frameContext.GetLightContainer();

        if (batch >= BatchType::ShadowMapLight0 && batch < BatchType::ShadowMapSun)
        {
            size_t shadowMapIndex = batch - BatchType::ShadowMapLight0;
            size_t lightIndex = shadowMapIndex / BLK_TEXCUBE_FACE_COUNT;
            return lightIndex < lightContainer.GetLights().size();
        }

        return true;
    }

    void BatchManager::GetBatchRange(BatchType batch, const Scene& scene, UINT& startIndex,
                                     UINT& endIndex)
    {
        switch (batch)
        {
        case BatchType::Transparent:
            startIndex = scene.GetOpaqueObjectCount();
            endIndex = scene.GetObjectCount();
            break;
        default:
            startIndex = 0;
            endIndex = scene.GetOpaqueObjectCount();
            break;
        }
    }

    void BatchManager::GetBatchView(BatchType batch, const RenderFrameContext& frameContext,
                                    Vector3& cameraCoord, Matrix4x4& viewProjMatrix)
    {
        const auto& lightContainer = frameContext.GetLightContainer();

        switch (batch)
        {
        case BatchType::Opaque:
        case BatchType::Transparent:
            viewProjMatrix = frameContext.GetViewProjMatrix();
            cameraCoord = frameContext.GetCameraPos();
            break;
        case BatchType::ShadowMapSun:
            viewProjMatrix = lightContainer.GetSunView() * lightContainer.GetSunProj();
            cameraCoord = lightContainer.GetSun().worldPos;
            break;
        default: {
            size_t shadowMapIndex = batch - BatchType::ShadowMapLight0;
            size_t lightIndex = shadowMapIndex / BLK_TEXCUBE_FACE_COUNT;
            size_t faceIndex = shadowMapIndex % BLK_TEXCUBE_FACE_COUNT;
            cameraCoord = lightContainer.GetLights()[lightIndex].worldPos;
            viewProjMatrix = lightContainer.GetViewProjMatrices()[lightIndex][faceIndex];
        }
        break;
        }
    }

    void BatchManager::CullObjects(const Scene& scene, Matrix4x4 viewProjMatrix,
                                   Vector3 cameraCoord, UINT startIndex, UINT endIndex,
                                   std::vector<SortingData>& culledObjects)
    {
        BLK_ASSERT(startIndex < endIndex);

        const auto& objects = scene.GetObjects();
        UINT batchObjectCount = endIndex - startIndex;
        culledObjects.reserve(batchObjectCount);
        Frustum calculatedFrustum(viewProjMatrix);
        for (UINT i = startIndex; i < endIndex; ++i)
        {
            const auto& objectAABB = objects[i].boundingBox;
            if (calculatedFrustum.CheckAABB(objectAABB))
            {
                Vector3 objectCoord = (objectAABB.GetMin() + objectAABB.GetMax()) / 2.0f;
                float distance = (objectCoord - cameraCoord).LengthSqr();
                culledObjects.push_back({i, distance});
            }
        }
    }

    void BatchManager::SortObjects(BatchType batch, std::vector<SortingData> culledObjects)
    {
        auto frontToBackPredicate = [](const SortingData& a, const SortingData& b) -> bool {
            return a.distance < b.distance;
        };

        auto backToFrontPredicate = [](const SortingData& a, const SortingData& b) -> bool {
            return a.distance > b.distance;
        };

        switch (batch)
        {
        case BatchType::Transparent:
            std::sort(culledObjects.begin(), culledObjects.end(), backToFrontPredicate);
            break;
        default:
            std::sort(culledObjects.begin(), culledObjects.end(), frontToBackPredicate);
            break;
        }
    }

    void BatchManager::GenerateDrawData(BatchType batch, const Scene& scene,
                                        const std::vector<SortingData>& culledObjects)
    {
        const auto& objects = scene.GetObjects();
        m_batches[static_cast<size_t>(batch)].resize(culledObjects.size());
        for (size_t i = 0; i < culledObjects.size(); ++i)
        {
            const auto& sceneObject = objects[culledObjects[i].objectIndex];
            m_batches[static_cast<size_t>(batch)][i] =
                DrawData{sceneObject.indexCount, sceneObject.startIndex};
        }
    }

} // namespace Boolka
