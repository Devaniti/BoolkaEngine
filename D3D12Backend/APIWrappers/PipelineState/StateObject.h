#pragma once
#include "APIWrappers/PipelineState/StateObjectParameters.h"

namespace Boolka
{
    class Device;
    class RootSignature;

    class StateObject
    {
    public:
        StateObject();
        ~StateObject();

        ID3D12StateObject* Get();
        ID3D12StateObject* operator->();

        template <typename... Args>
        bool Initialize(Device& device, const Args&... args)
        {
            D3D12_STATE_SUBOBJECT subobjects[sizeof...(Args)];

            StateObjectStream stateObjectStream(subobjects, args...);

            D3D12_STATE_OBJECT_DESC desc;
            desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
            desc.NumSubobjects = sizeof...(Args);
            desc.pSubobjects = subobjects;
            return InitializeInternal(device, desc);
        };

        void Unload();

    private:
        bool InitializeInternal(Device& device, const D3D12_STATE_OBJECT_DESC& desc);

        ID3D12StateObject* m_StateObject;
    };

    BLK_IS_PLAIN_DATA_ASSERT(D3D12_GRAPHICS_PIPELINE_STATE_DESC);

} // namespace Boolka
