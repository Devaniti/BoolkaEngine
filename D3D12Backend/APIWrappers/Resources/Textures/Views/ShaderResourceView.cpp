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

    bool ShaderResourceView::Initialize(Device& device, Texture2D& texture,
                                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);

        device->CreateShaderResourceView(texture.Get(), nullptr, destDescriptor);
        m_CPUDescriptorHandle = destDescriptor;

        return true;
    }

    bool ShaderResourceView::Initialize(Device& device, Texture2D& texture,
                                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor,
                                        DXGI_FORMAT format)
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = -1;
        device->CreateShaderResourceView(texture.Get(), &srvDesc, destDescriptor);
        m_CPUDescriptorHandle = destDescriptor;

        return true;
    }

    bool ShaderResourceView::InitializeCube(Device& device, Texture2D& texture,
                                            D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor,
                                            DXGI_FORMAT format)
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.Texture2D.MipLevels = -1;
        device->CreateShaderResourceView(texture.Get(), &srvDesc, destDescriptor);
        m_CPUDescriptorHandle = destDescriptor;

        return true;
    }

    void ShaderResourceView::Unload()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0);
        m_CPUDescriptorHandle = {};
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* ShaderResourceView::GetCPUDescriptor()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0);
        return &m_CPUDescriptorHandle;
    }

} // namespace Boolka