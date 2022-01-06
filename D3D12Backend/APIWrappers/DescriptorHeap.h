#pragma once

namespace Boolka
{

    class Device;

    class [[nodiscard]] DescriptorHeap
    {
    public:
        DescriptorHeap();
        ~DescriptorHeap();

        [[nodiscard]] ID3D12DescriptorHeap* Get();
        [[nodiscard]] ID3D12DescriptorHeap* operator->();
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const;
        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index) const;

        bool Initialize(Device& device, UINT elementCount, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
                        D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags);
        void Unload();

    private:
        ID3D12DescriptorHeap* m_DescriptorHeap;
        UINT m_DescriptorHandleIncrementSize;
        D3D12_CPU_DESCRIPTOR_HANDLE m_CPUStartHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE m_GPUStartHandle;

#ifdef BLK_DEBUG
        UINT m_ElementCount;
#endif
    };

} // namespace Boolka
