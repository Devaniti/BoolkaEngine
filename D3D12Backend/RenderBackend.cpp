#include "stdafx.h"
#include "RenderBackend.h"

#include "RenderBackendImpl.h"

namespace Boolka
{

    RenderBackend* RenderBackend::CreateRenderBackend()
    {
        return new RenderBackendImpl();
    }

    void RenderBackend::DeleteRenderBackend(RenderBackend* object)
    {
        delete object;
    }

}

