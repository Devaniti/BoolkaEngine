#include "stdafx.h"
#include "DescriptorHeap.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    DescriptorHeap::DescriptorHeap()
        : m_DescriptorHeap(nullptr)
        , m_DescriptorHandleIncrementSize(0)
        , m_CPUStartHandle()
        , m_GPUStartHandle()
#ifdef BLK_DEBUG
        , m_ElementCount(0)
#endif
    {
    }

    DescriptorHeap::~DescriptorHeap()
    {
        BLK_ASSERT(m_DescriptorHeap == nullptr);
        BLK_ASSERT(m_DescriptorHandleIncrementSize == 0);
        BLK_ASSERT(m_CPUStartHandle.ptr == 0);
        BLK_ASSERT(m_GPUStartHandle.ptr == 0);
#ifdef BLK_DEBUG
        BLK_ASSERT(m_ElementCount == 0);
#endif
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(UINT index)
    {
#ifdef BLK_DEBUG
        BLK_ASSERT(m_CPUStartHandle.ptr != 0);
        BLK_ASSERT(index < m_ElementCount);
#endif

        D3D12_CPU_DESCRIPTOR_HANDLE result = m_CPUStartHandle;
        result.ptr += static_cast<SIZE_T>(index) * m_DescriptorHandleIncrementSize;
        return result;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(UINT index)
    {
#ifdef BLK_DEBUG
        BLK_ASSERT(m_GPUStartHandle.ptr != 0);
        BLK_ASSERT(index < m_ElementCount);
#endif

        D3D12_GPU_DESCRIPTOR_HANDLE result = m_GPUStartHandle;
        result.ptr += static_cast<UINT64>(index) * m_DescriptorHandleIncrementSize;
        return result;
    }

    bool DescriptorHeap::Initialize(Device& device, UINT elementCount, D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags)
    {
        BLK_ASSERT(m_DescriptorHeap == nullptr);

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = elementCount;
        rtvHeapDesc.Type = heapType;
        rtvHeapDesc.Flags = heapFlags;
        HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
        if (!SUCCEEDED(hr)) return false;

        m_DescriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(heapType);

        m_CPUStartHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

        if (heapFlags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
        {
            m_GPUStartHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        }
        else
        {
            m_GPUStartHandle.ptr = 0;
        }

#ifdef BLK_DEBUG
        m_ElementCount = elementCount;
#endif

        return true;
    }

    void DescriptorHeap::Unload()
    {
        BLK_ASSERT(m_DescriptorHeap != nullptr);

        m_DescriptorHeap->Release();
        m_DescriptorHeap = nullptr;

        m_DescriptorHandleIncrementSize = 0;
        m_CPUStartHandle = {};
        m_GPUStartHandle = {};

#ifdef BLK_DEBUG
        m_ElementCount = 0;
#endif
    }

}