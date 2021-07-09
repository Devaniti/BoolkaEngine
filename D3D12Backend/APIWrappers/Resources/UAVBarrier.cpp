#include "stdafx.h"

#include "UAVBarrier.h"

#include "APIWrappers/CommandList/CommandList.h"
#include "APIWrappers/Resources/Resource.h"

namespace Boolka
{

    void UAVBarrier::Barrier(CommandList& commangList, Resource& resource)
    {
        D3D12_RESOURCE_BARRIER uavBarrier{};
        uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        uavBarrier.UAV.pResource = resource.Get();
        commangList->ResourceBarrier(1, &uavBarrier);
    }

    void UAVBarrier::BarrierAll(CommandList& commangList)
    {
        D3D12_RESOURCE_BARRIER uavBarrier{};
        uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        commangList->ResourceBarrier(1, &uavBarrier);
    }

} // namespace Boolka
