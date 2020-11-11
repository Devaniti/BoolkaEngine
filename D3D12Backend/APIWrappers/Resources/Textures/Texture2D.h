#pragma once
#include "Texture.h"

namespace Boolka
{

    class Device;

    class Texture2D : public Texture
    {
    public:
        Texture2D();
        ~Texture2D();

        // TODO
        bool Initialize(Device& device, D3D12_HEAP_TYPE heapType, UINT64 width, UINT height, UINT16 mipCount, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags, D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES initialState);

        // This method don't increment resource's reference count
        // You should do it yourself if needed
        bool Initialize(ID3D12Resource* resource);
        void Unload();

    private:
    };

}
