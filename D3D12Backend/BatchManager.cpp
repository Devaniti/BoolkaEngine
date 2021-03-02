#include "stdafx.h"
#include "BatchManager.h"
#include "Containers/Scene.h"
#include "APIWrappers/CommandList/CommandList.h"
#include "Contexts/RenderFrameContext.h"
#include "BoolkaCommon/Structures/Frustum.h"

namespace Boolka
{

    BLK_DEFINE_ENUM_OPERATOR_PLUS(BatchManager::BatchType);
    BLK_DEFINE_ENUM_OPERATOR_MINUS(BatchManager::BatchType);

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
        GetBatch(BatchType::Transparent).reserve(scene.GetObjectCount() - scene.GetOpaqueObjectCount());
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

    bool BatchManager::PrepareBatches(RenderFrameContext& frameContext, Scene& scene)
    {
        const auto& objects = scene.GetObjects();
        const auto& lightContainer = frameContext.GetLightContainer();

        for (BatchType batchType = BatchType::Opaque; batchType < BatchType::Count; batchType = static_cast<BatchType>(static_cast<int>(batchType) + 1))
        {
            UINT startIndex, endIndex;
            switch (batchType)
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

            Vector3 cameraCoord;

            Matrix4x4 viewProjMatrix;
            switch (batchType)
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
            default:
            {
                size_t shadowMapIndex = batchType - BatchType::ShadowMapLight0;
                size_t lightIndex = shadowMapIndex / BLK_TEXCUBE_FACE_COUNT;
                size_t faceIndex = shadowMapIndex % BLK_TEXCUBE_FACE_COUNT;
                if (lightIndex >= lightContainer.GetLights().size())
                    continue;
                cameraCoord = lightContainer.GetLights()[lightIndex].worldPos;
                viewProjMatrix = lightContainer.GetViewProjMatrices()[lightIndex][faceIndex];
            }
                break;

            }

            Frustum calculatedFrustum(viewProjMatrix);

            // used to sort objects by distance
            struct tempObject
            {
                UINT objectIndex;
                float distance;
            };

            // Index of visible object / distance to camera
            std::vector <tempObject> tempObjects;
            UINT batchObjectCount = endIndex - startIndex;
            tempObjects.reserve(batchObjectCount);

            for (UINT i = startIndex; i < endIndex; ++i)
            {
                const auto& objectAABB = objects[i].boundingBox;
                if (calculatedFrustum.CheckAABB(objectAABB))
                {
                    Vector3 objectCoord = (objectAABB.GetMin() + objectAABB.GetMax()) / 2.0f;
                    float distance = (objectCoord - cameraCoord).LengthSlow();
                    tempObjects.push_back({ i, distance });
                }
            }

            auto frontToBackPredicate = [](const tempObject& a, const tempObject& b) -> bool
            {
                return a.distance < b.distance;
            };

            auto backToFrontPredicate = [](const tempObject& a, const tempObject& b) -> bool
            {
                return a.distance > b.distance;
            };

            switch (batchType)
            {
            case BatchType::Transparent:
                std::sort(tempObjects.begin(), tempObjects.end(), backToFrontPredicate);
                break;
            default:
                std::sort(tempObjects.begin(), tempObjects.end(), frontToBackPredicate);
                break;
            }

            m_batches[static_cast<size_t>(batchType)].resize(tempObjects.size());
            for (size_t i = 0; i < tempObjects.size(); ++i)
            {
                const auto& sceneObject = objects[tempObjects[i].objectIndex];
                m_batches[static_cast<size_t>(batchType)][i] = DrawData{ sceneObject.indexCount, sceneObject.startIndex };
            }

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

}
