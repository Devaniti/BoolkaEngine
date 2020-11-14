#pragma once
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    class Device;
    class Resource;
    class Texture2D;
    class CommandList;
    class RenderTargetView;

    class ResourceTracker
    {
    public:
        ResourceTracker() = default;
        ~ResourceTracker();

        bool Initialize(Device& device, size_t expectedResources);
        void Unload();

        void RegisterResource(Resource& resource, D3D12_RESOURCE_STATES initialState);
        bool Transition(Resource& resource, CommandList& commandList, D3D12_RESOURCE_STATES targetState);

        void Decay();

    private:

        std::unordered_map<Resource*, D3D12_RESOURCE_STATES> m_TrackedResources;
    };

}
