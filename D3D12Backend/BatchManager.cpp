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
        : m_requiredBatches{}
    {
        // By default all batches are required
        std::fill(std::begin(m_requiredBatches), std::end(m_requiredBatches), true);
    };

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
        CalculateRequiredBatches(frameContext);

        for (BatchType batch = BatchType::Opaque; batch < BatchType::Count; ++batch)
        {
            if (!NeedRender(batch))
            {
                ClearBatch(batch);
                continue;
            }

            UINT startIndex, endIndex;
            GetBatchRange(batch, scene, startIndex, endIndex);

            Vector4 cameraCoord;
            Matrix4x4 viewProjMatrix;
            GetBatchView(batch, frameContext, cameraCoord, viewProjMatrix);

            std::vector<SortingData> culledObjects;
            CullObjects(scene, viewProjMatrix, cameraCoord, startIndex, endIndex, culledObjects);

            SortObjects(batch, culledObjects);

            GenerateDrawData(batch, scene, culledObjects);
        }

        return true;
    }

    bool BatchManager::NeedRender(BatchType batch) const
    {
        return m_requiredBatches[static_cast<size_t>(batch)];
    }

    bool BatchManager::Render(CommandList& commandList, BatchType batch) const
    {
        BLK_ASSERT(batch < BatchType::Count);
        for (const auto& object : GetBatch(batch))
        {
            commandList->DrawIndexedInstanced(object.indexCount, 1, object.startIndex, 0, 0);
        }
        return true;
    }

    size_t BatchManager::GetCount(BatchType batch)
    {
        return GetBatch(batch).size();
    }

    bool BatchManager::IsBatchEnabled(BatchType batch, const RenderFrameContext& frameContext)
    {
        BLK_ASSERT(batch < BatchType::Count);
        const auto& lightContainer = frameContext.GetLightContainer();

        if (batch >= BatchType::ShadowMapLight0 && batch < BatchType::ShadowMapSun)
        {
            size_t shadowMapIndex = batch - BatchType::ShadowMapLight0;
            size_t lightIndex = shadowMapIndex / BLK_TEXCUBE_FACE_COUNT;
            if (lightIndex >= lightContainer.GetLights().size())
                return false;
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
                                    Vector4& cameraCoord, Matrix4x4& viewProjMatrix)
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
                                   Vector4 cameraCoord, UINT startIndex, UINT endIndex,
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
            if (calculatedFrustum.CheckAABBFast(objectAABB))
            {
                Vector4 objectCoord = (objectAABB.GetMin() + objectAABB.GetMax()) / 2.0f;
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

    std::vector<BatchManager::DrawData>& BatchManager::GetBatch(BatchType id)
    {
        return m_batches[static_cast<size_t>(id)];
    }

    const std::vector<BatchManager::DrawData>& BatchManager::GetBatch(BatchType id) const
    {
        return m_batches[static_cast<size_t>(id)];
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

        m_requiredBatches[static_cast<size_t>(batch)] = true;
    }

    void BatchManager::ClearBatch(BatchType batch)
    {
        GetBatch(batch).resize(0);
        m_requiredBatches[static_cast<size_t>(batch)] = false;
    }

    void BatchManager::CalculateRequiredBatches(const RenderFrameContext& frameContext)
    {
        const auto& lightContainer = frameContext.GetLightContainer();
        Frustum mainCameraFrustum(frameContext.GetViewProjMatrix());

#ifdef BLK_ENABLE_STATS
        auto& frameStats = frameContext.GetFrameStats();
        frameStats.renderedLights = 0;
        frameStats.renderedLightFrustums = 0;
#endif

        for (size_t i = 0; i < lightContainer.GetLights().size(); ++i)
        {
            const auto& light = lightContainer.GetLights()[i];
            Vector4 halfExtent{light.farZ, light.farZ, light.farZ, 0.0f};
            Vector4 lightPos = Vector4(light.worldPos, 1.0f);
            AABB lightAABB = AABB(lightPos - halfExtent, lightPos + halfExtent);
            // Cull whole light
            if (!mainCameraFrustum.CheckSphereFast(lightPos, light.farZ))
            {
                for (size_t j = 0; j < BLK_TEXCUBE_FACE_COUNT; ++j)
                {
                    BatchType batch = BatchType::ShadowMapLight0 + i * BLK_TEXCUBE_FACE_COUNT + j;
                    m_requiredBatches[static_cast<size_t>(batch)] = false;
                }
                continue;
            }

#ifdef BLK_ENABLE_STATS
            frameStats.renderedLights++;
#endif

            // Then cull each frustum separately
            for (size_t j = 0; j < BLK_TEXCUBE_FACE_COUNT; ++j)
            {
                BatchType batch = BatchType::ShadowMapLight0 + i * BLK_TEXCUBE_FACE_COUNT + j;
                const auto& viewMatrix = lightContainer.GetViewMatrices()[i][j];
                const auto& projMatrix = lightContainer.GetProjMatrices()[i][j];
                bool isSuccessfull{};
                Matrix4x4 invViewMatrix = viewMatrix.Inverse(isSuccessfull);
                BLK_ASSERT(isSuccessfull);
                Matrix4x4 invProjMatrix = projMatrix.Inverse(isSuccessfull);
                BLK_ASSERT(isSuccessfull);
                m_requiredBatches[static_cast<size_t>(batch)] =
                    mainCameraFrustum.CheckFrustumFast(invViewMatrix, invProjMatrix);

#ifdef BLK_ENABLE_STATS
                if (m_requiredBatches[static_cast<size_t>(batch)])
                    frameStats.renderedLightFrustums++;
#endif
            }
        }

        // Mark all batches of out lights out of bounds as non required
        for (size_t i = lightContainer.GetLights().size(); i < BLK_MAX_LIGHT_COUNT; ++i)
        {
            for (size_t j = 0; j < BLK_TEXCUBE_FACE_COUNT; ++j)
            {
                BatchType batch = BatchType::ShadowMapLight0 + i * BLK_TEXCUBE_FACE_COUNT + j;
                m_requiredBatches[static_cast<size_t>(batch)] = false;
            }
        }
    }

} // namespace Boolka
