#include "stdafx.h"

#include "ResourceTransition.h"

#include "APIWrappers/CommandList/CommandList.h"
#include "APIWrappers/Resources/Resource.h"

namespace Boolka
{

    void ResourceTransition::Transition(CommandList& commangList, Resource& resource,
                                        D3D12_RESOURCE_STATES srcState,
                                        D3D12_RESOURCE_STATES dstState)
    {
        BLK_ASSERT(srcState != dstState);

        D3D12_RESOURCE_BARRIER resourceBarrier = {};
        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        resourceBarrier.Transition.pResource = resource.Get();
        resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        resourceBarrier.Transition.StateBefore = srcState;
        resourceBarrier.Transition.StateAfter = dstState;
        commangList->ResourceBarrier(1, &resourceBarrier);
    }

    void ResourceTransition::TransitionMany(
        CommandList& commangList,
        std::tuple<ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES>* transitions,
        size_t transitionCount)
    {
        std::vector<D3D12_RESOURCE_BARRIER> barriers(transitionCount);
        for (size_t i = 0; i < transitionCount; ++i)
        {
            barriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barriers[i].Transition.pResource = std::get<0>(transitions[i]);
            barriers[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barriers[i].Transition.StateBefore = std::get<1>(transitions[i]);
            barriers[i].Transition.StateAfter = std::get<2>(transitions[i]);
        }

        commangList->ResourceBarrier(static_cast<UINT>(transitionCount), barriers.data());
    }

    void ResourceTransition::TransitionMany(CommandList& commangList, Resource** resources,
                                            D3D12_RESOURCE_STATES sharedSrcState,
                                            D3D12_RESOURCE_STATES sharedDstState,
                                            size_t transitionCount)
    {
        std::vector<D3D12_RESOURCE_BARRIER> barriers(transitionCount);
        for (size_t i = 0; i < transitionCount; ++i)
        {
            barriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barriers[i].Transition.pResource = resources[i]->Get();
            barriers[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barriers[i].Transition.StateBefore = sharedSrcState;
            barriers[i].Transition.StateAfter = sharedDstState;
        }

        commangList->ResourceBarrier(static_cast<UINT>(transitionCount), barriers.data());
    }

    bool ResourceTransition::NeedTransition(D3D12_RESOURCE_STATES srcState,
                                            D3D12_RESOURCE_STATES dstState)
    {
        return srcState != dstState;
    }

    bool ResourceTransition::CanPromote(D3D12_RESOURCE_STATES srcState,
                                        D3D12_RESOURCE_STATES dstState)
    {
#ifdef BLK_USE_COMMON_STATE_PROMOTION
        // TODO rewrite check to be stricter as described in
        // https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12#implicit-state-transitions

        static const D3D12_RESOURCE_STATES nonPromotableStates = D3D12_RESOURCE_STATE_DEPTH_WRITE |
                                                                 D3D12_RESOURCE_STATE_DEPTH_READ |
                                                                 D3D12_RESOURCE_STATE_RENDER_TARGET;

        bool isCommonState = srcState == D3D12_RESOURCE_STATE_COMMON;
        bool onlyOneBit = BLK_IS_POWER_OF_TWO(dstState);
        bool isPromotableState = (dstState & nonPromotableStates) == 0;

        return isCommonState && onlyOneBit && isPromotableState;
#else
        return false;
#endif
    }

    bool ResourceTransition::CanDecay(D3D12_RESOURCE_STATES srcState)
    {
#ifdef BLK_USE_COMMON_STATE_PROMOTION
        // TODO rewrite check to be stricter as described in
        // https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12#implicit-state-transitions

        static const D3D12_RESOURCE_STATES nonPromotableStates =
            D3D12_RESOURCE_STATE_DEPTH_WRITE | D3D12_RESOURCE_STATE_DEPTH_READ;
        bool isPromotableState = (srcState & nonPromotableStates) == 0;

        return isPromotableState;
#else
        return false;
#endif
    }

} // namespace Boolka
