#include "stdafx.h"
#include "CullingManager.h"
#include "Containers/Scene.h"
#include "APIWrappers/CommandList/CommandList.h"
#include "Contexts/RenderEngineContext.h"
#include "Contexts/RenderFrameContext.h"
#include "Contexts/RenderThreadContext.h"
#include "BoolkaCommon/Structures/Frustum.h"
#include "BoolkaCommon/DebugHelpers/DebugOutputStream.h"

namespace Boolka
{

    CullingManager::~CullingManager()
    {
        BLK_ASSERT(m_visibility.empty());
    }

    bool CullingManager::Initialize(UINT objectCount)
    {
        m_visibility.resize(objectCount);
        return true;
    }

    void CullingManager::Unload()
    {
        m_visibility.clear();
    }

    bool CullingManager::Cull(RenderFrameContext& frameContext, Scene& scene)
    {
        Matrix4x4 viewProjMatrix = frameContext.GetViewMatrix() * frameContext.GetProjMatrix();

        Frustum calculatedFrustum(viewProjMatrix);

        size_t visibleCount = 0;

        const auto& objects = scene.GetObjects();
        for (size_t i = 0; i < objects.size(); ++i)
        {
            const auto& object = objects[i];
            m_visibility[i] = calculatedFrustum.CheckAABB(object.boundingBox) != Frustum::Outside;
            if (m_visibility[i])
                visibleCount++;
        }

        g_WDebugOutput << L"Visible " << visibleCount << std::endl;

        return true;
    }

    bool CullingManager::Render(Scene& scene, CommandList& commandList)
    {
        const auto& objects = scene.GetObjects();
        BLK_ASSERT(objects.size() == m_visibility.size());

        for (size_t i = 0; i < objects.size(); ++i)
        {
            if (m_visibility[i])
            {
                const auto& object = objects[i];
                commandList->DrawIndexedInstanced(object.indexCount, 1, object.startIndex, 0, 0);
            }
        }

        return true;
    }

}
