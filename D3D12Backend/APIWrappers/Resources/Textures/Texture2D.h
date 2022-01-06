#pragma once
#include "Texture.h"

namespace Boolka
{

    class Device;
    class ResourceHeap;

    class [[nodiscard]] Texture2D : public Texture
    {
    public:
        Texture2D() = default;
        ~Texture2D() = default;

        // Creates committed resource
        bool Initialize(Device& device, D3D12_HEAP_TYPE heapType, UINT64 width, UINT height,
                        UINT16 mipCount, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags,
                        D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES initialState,
                        UINT16 arraySize = 1);
        // Creates placed resource
        bool Initialize(Device& device, ResourceHeap& resourceHeap, size_t heapOffset, UINT64 width,
                        UINT height, UINT16 mipCount, DXGI_FORMAT format,
                        D3D12_RESOURCE_FLAGS resourceFlags, D3D12_CLEAR_VALUE* clearValue,
                        D3D12_RESOURCE_STATES initialState, UINT16 arraySize = 1);
        static void GetRequiredSize(size_t& outAlignment, size_t& outSize, Device& device,
                                    UINT64 width, UINT height, UINT16 mipCount, DXGI_FORMAT format,
                                    D3D12_RESOURCE_FLAGS resourceFlags, UINT16 arraySize = 1);
        [[nodiscard]] static size_t GetUploadSize(UINT width, UINT height, UINT16 mipCount,
                                                  DXGI_FORMAT format,
                                                  D3D12_RESOURCE_FLAGS resourceFlags,
                                                  UINT16 arraySize = 1);
        [[nodiscard]] static UINT GetBPP(DXGI_FORMAT format);

        // This method don't increment resource's reference count
        // You should do it yourself if needed
        bool Initialize(ID3D12Resource* resource);
        void Unload();

    private:
        static D3D12_RESOURCE_DESC FillDesc(UINT64 width, UINT height, UINT16 mipCount,
                                            DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags,
                                            UINT16 arraySize);
    };

} // namespace Boolka
