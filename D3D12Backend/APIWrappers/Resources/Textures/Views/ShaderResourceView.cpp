#include "stdafx.h"

#include "ShaderResourceView.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"

namespace Boolka
{

    void ShaderResourceView::Initialize(Device& device, Texture2D& texture,
                                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        device->CreateShaderResourceView(texture.Get(), nullptr, destDescriptor);
    }

    void ShaderResourceView::Initialize(Device& device, Texture2D& texture,
                                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor,
                                        DXGI_FORMAT format)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = -1;
        device->CreateShaderResourceView(texture.Get(), &srvDesc, destDescriptor);
    }

    void ShaderResourceView::Initialize(Device& device, Buffer& buffer, UINT elementCount,
                                        UINT stride, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        Initialize(device, buffer, elementCount, stride, DXGI_FORMAT_UNKNOWN, destDescriptor);
    }

    void ShaderResourceView::Initialize(Device& device, Buffer& buffer, UINT elementCount,
                                        DXGI_FORMAT format,
                                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        Initialize(device, buffer, elementCount, 0, format, destDescriptor);
    }

    void ShaderResourceView::InitializeCube(Device& device, Texture2D& texture,
                                            D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor,
                                            DXGI_FORMAT format)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.Texture2D.MipLevels = -1;
        device->CreateShaderResourceView(texture.Get(), &srvDesc, destDescriptor);
    }

    void ShaderResourceView::InitializeAccelerationStructure(
        Device& device, D3D12_GPU_VIRTUAL_ADDRESS tlasAddress,
        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
        srvDesc.RaytracingAccelerationStructure.Location = tlasAddress;
        device->CreateShaderResourceView(nullptr, &srvDesc, destDescriptor);
    }

    void ShaderResourceView::InitializeNullDescriptorTexture2D(
        Device& device, DXGI_FORMAT format, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        device->CreateShaderResourceView(nullptr, &srvDesc, destDescriptor);
    }

    void ShaderResourceView::Initialize(Device& device, Buffer& buffer, UINT elementCount,
                                        UINT stride, DXGI_FORMAT format,
                                        D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = elementCount;
        srvDesc.Buffer.StructureByteStride = stride;
        device->CreateShaderResourceView(buffer.Get(), &srvDesc, destDescriptor);
    }

} // namespace Boolka