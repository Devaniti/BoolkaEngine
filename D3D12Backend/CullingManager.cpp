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

    bool CullingManager::Cull(RenderFrameContext& frameContext, Scene& scene)
    {
        Matrix4x4 viewProjMatrix = frameContext.GetViewMatrix() * frameContext.GetProjMatrix();

        Frustum calculatedFrustum(viewProjMatrix);

#ifdef BLK_ENABLE_STATS
        size_t insideFrustum = 0;
        size_t intersectFrustum = 0;
#endif

        const auto& objects = scene.GetObjects();
        for (size_t i = 0; i < objects.size(); ++i)
        {
            const auto& object = objects[i];
            Frustum::TestResult testResult = calculatedFrustum.CheckAABB(object.boundingBox);
            m_visibility[i] = testResult != Frustum::Outside;

#ifdef BLK_ENABLE_STATS
            switch (testResult)
            {
            case Frustum::Intersects:
                ++insideFrustum;
                break;
            case Frustum::Inside:
                ++intersectFrustum;
                break;
            }
#endif
        }


#ifdef BLK_ENABLE_STATS
        size_t outsideFrustum = objects.size() - insideFrustum - intersectFrustum;
        frameContext.GetFrameStats().insideFrustum = insideFrustum;
        frameContext.GetFrameStats().intersectFrustum = intersectFrustum;
        frameContext.GetFrameStats().outsideFrustum = outsideFrustum;
#endif

        return true;
    }

}
