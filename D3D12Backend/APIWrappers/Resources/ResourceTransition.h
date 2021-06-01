#pragma once

namespace Boolka
{

    class Resource;
    class CommandList;

    class ResourceTransition
    {
    public:
        static void Transition(CommandList& commangList, Resource& resource,
                               D3D12_RESOURCE_STATES srcState, D3D12_RESOURCE_STATES dstState);

        static void TransitionMany(
            CommandList& commangList,
            std::tuple<ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES>* transitions,
            size_t transitionCount);

        static void TransitionMany(CommandList& commangList, Resource** resources,
                                   D3D12_RESOURCE_STATES sharedSrcState,
                                   D3D12_RESOURCE_STATES sharedDstState, size_t transitionCount);

        static bool NeedTransition(D3D12_RESOURCE_STATES srcState, D3D12_RESOURCE_STATES dstState);

        // Common state promotion/decay helpers
        static bool CanPromote(D3D12_RESOURCE_STATES srcState, D3D12_RESOURCE_STATES dstState);
        static bool CanDecay(D3D12_RESOURCE_STATES srcState);
    };

} // namespace Boolka
