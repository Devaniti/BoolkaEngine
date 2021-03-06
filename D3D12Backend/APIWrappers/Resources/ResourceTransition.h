#pragma once

namespace Boolka
{

    class Resource;
    class CommandList;

    class ResourceTransition
    {
    public:
        static bool Transition(Resource& resource, CommandList& commangList,
                               D3D12_RESOURCE_STATES srcState, D3D12_RESOURCE_STATES dstState);

        static bool NeedTransition(D3D12_RESOURCE_STATES srcState, D3D12_RESOURCE_STATES dstState);

        // Common state promotion/decay helpers
        static bool CanPromote(D3D12_RESOURCE_STATES srcState, D3D12_RESOURCE_STATES dstState);
        static bool CanDecay(D3D12_RESOURCE_STATES srcState);
    };

} // namespace Boolka
