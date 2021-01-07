#include "stdafx.h"
#include "ConstantBufferView.h"
#include "APIWrappers/Device.h"

namespace Boolka
{

    ConstantBufferView::ConstantBufferView()
        : m_CPUDescriptorHandle{}
    {
    }

    ConstantBufferView::~ConstantBufferView()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);
    }

    bool ConstantBufferView::Initialize(Device& device, Buffer& constantBuffer, D3D12_CPU_DESCRIPTOR_HANDLE destinationDescriptorHandle, UINT size)
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);
        D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
        desc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
        desc.SizeInBytes = size;
        device->CreateConstantBufferView(&desc, destinationDescriptorHandle);
        m_CPUDescriptorHandle = destinationDescriptorHandle;

        return true;
    }

    void ConstantBufferView::Unload()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0);
        m_CPUDescriptorHandle = {};
    }

}