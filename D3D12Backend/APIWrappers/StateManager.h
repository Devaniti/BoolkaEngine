#pragma once
#include "DescriptorHeap.h"
#include "RootSignature.h"

namespace Boolka
{

    class StateManager
    {
    public:
        StateManager() = default;
        ~StateManager() = default;

        bool Initialize(Device& device);
        void Unload();

    private:
        DescriptorHeap m_DescriptorHeapSRV;
    };

} // namespace Boolka
