#pragma once
#include "RootSignature.h"
#include "DescriptorHeap.h"

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

}
