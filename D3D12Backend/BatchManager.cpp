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
        CalculateRequiredBatches(frameContext);

        return true;
    }

    bool BatchManager::NeedRender(BatchType batch) const
    {
        return m_requiredBatches[static_cast<size_t>(batch)];
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
