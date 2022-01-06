#pragma once
#include "APIWrappers/Resources/Resource.h"

namespace Boolka
{

    class Device;

    class [[nodiscard]] Buffer : public Resource
    {
    public:
        Buffer() = default;
        ~Buffer() = default;

        bool Initialize(Device& device, UINT64 size, D3D12_HEAP_TYPE heapType,
                        D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialState);
        void Unload();
    };

} // namespace Boolka
