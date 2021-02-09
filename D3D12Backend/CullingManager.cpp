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

    bool CullingManager::Initialize(const Scene& scene)
    {
        m_visibility.resize(scene.GetObjectCount());
        return true;
    }

    void CullingManager::Unload()
    {
        m_visibility.clear();
    }

    bool CullingManager::Cull(const RenderFrameContext& frameContext, Scene& scene)
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

}
