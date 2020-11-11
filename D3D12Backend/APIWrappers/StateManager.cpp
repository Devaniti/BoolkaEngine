#include "stdafx.h"
#include "StateManager.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    StateManager::StateManager()
    {
    }

    StateManager::~StateManager()
    {
    }

    bool StateManager::Initialize(Device& device)
    {
        if (!m_DescriptorHeapSRV.Initialize(device, 64, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)) return false;

        return true;
    }

    void StateManager::Unload()
    {
        m_DescriptorHeapSRV.Unload();
    }

}