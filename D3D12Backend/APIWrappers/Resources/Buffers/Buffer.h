#pragma once
#include "APIWrappers/Resources/Resource.h"

namespace Boolka
{

    class Device;

    class Buffer :
        public Resource
    {
    protected:
        bool InitializeCommitedResource(Device& device, UINT64 size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState);

        Buffer();
        ~Buffer();
    };

}
