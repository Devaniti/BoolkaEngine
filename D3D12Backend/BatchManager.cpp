#include "stdafx.h"
#include "BatchManager.h"
#include "Containers/Scene.h"
#include "APIWrappers/CommandList/CommandList.h"
#include "Contexts/RenderFrameContext.h"

namespace Boolka
{

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
        m_batches[static_cast<size_t>(BatchType::Opaque)].reserve(scene.GetOpaqueObjectCount());
        m_batches[static_cast<size_t>(BatchType::Transparent)].reserve(scene.GetObjectCount() - scene.GetOpaqueObjectCount());
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
        auto& cullingManager = scene.GetCullingManager();
        const auto& objects = scene.GetObjects();

        cullingManager.Cull(frameContext, scene);

        const auto& visibility = cullingManager.GetVisibility();

        for (BatchType batchType = BatchType::Opaque; batchType < BatchType::Count; batchType = static_cast<BatchType>(static_cast<int>(batchType) + 1))
        {
            UINT startIndex, endIndex;
            switch (batchType)
            {
            case Boolka::BatchManager::BatchType::Opaque:
                startIndex = 0;
                endIndex = scene.GetOpaqueObjectCount();
                break;
            case Boolka::BatchManager::BatchType::Transparent:
                startIndex = scene.GetOpaqueObjectCount();
                endIndex = scene.GetObjectCount();
                break;
            default:
                BLK_ASSERT(0);
                break;
            }

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
                if (visibility[i])
                {
                    const auto& objectAABB = objects[i].boundingBox;
                    Vector3 objectCoord = (objectAABB.GetMin() + objectAABB.GetMax()) / 2.0f;
                    Vector3 cameraCoord = frameContext.GetCameraPos();
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
            case BatchType::Opaque:
                std::sort(tempObjects.begin(), tempObjects.end(), frontToBackPredicate);
                break;
            case BatchType::Transparent:
                std::sort(tempObjects.begin(), tempObjects.end(), backToFrontPredicate);
                break;
            default:
                BLK_ASSERT(0);
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
