#include "stdafx.h"
#include "ShaderResourceView.h"
#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"

namespace Boolka
{

    ShaderResourceView::ShaderResourceView()
        : m_CPUDescriptorHandle{}
    {
    }

    ShaderResourceView::~ShaderResourceView()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);
    }

    bool ShaderResourceView::Initialize(Device& device, Texture2D& texture, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);

        device->CreateShaderResourceView(texture.Get(), nullptr, destDescriptor);
        m_CPUDescriptorHandle = destDescriptor;

        return true;
    }

    void ShaderResourceView::Unload()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0);
        m_CPUDescriptorHandle = {};
    }
}