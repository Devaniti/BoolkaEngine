#include "stdafx.h"

#include "UnorderedAccessView.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"

namespace Boolka
{

    void UnorderedAccessView::Initialize(Device& device, Texture1D& texture, DXGI_FORMAT format,
                                         D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.Format = format;
        desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;

        device->CreateUnorderedAccessView(texture.Get(), nullptr, &desc, destDescriptor);
    }

    void UnorderedAccessView::Initialize(Device& device, Texture2D& texture, DXGI_FORMAT format,
                                         D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.Format = format;
        desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        device->CreateUnorderedAccessView(texture.Get(), nullptr, &desc, destDescriptor);
    }

    void UnorderedAccessView::Initialize(Device& device, Buffer& buffer, UINT stride,
                                         UINT elementCount,
                                         D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        Initialize(device, buffer, DXGI_FORMAT_UNKNOWN, stride, elementCount, destDescriptor);
    }

    void UnorderedAccessView::Initialize(Device& device, Buffer& buffer, DXGI_FORMAT format,
                                         UINT elementCount,
                                         D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        Initialize(device, buffer, format, 0, elementCount, destDescriptor);
    }

    void UnorderedAccessView::InitializeWithCounter(Device& device, Buffer& buffer, UINT stride,
                                                    UINT elementCount,
                                                    D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        InitializeWithCounter(device, buffer, DXGI_FORMAT_UNKNOWN, stride, elementCount,
                              destDescriptor);
    }

    void UnorderedAccessView::InitializeWithCounter(Device& device, Buffer& buffer,
                                                    DXGI_FORMAT format, UINT elementCount,
                                                    D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        InitializeWithCounter(device, buffer, format, 0, elementCount, destDescriptor);
    }

    void UnorderedAccessView::Initialize(Device& device, Buffer& buffer, DXGI_FORMAT format,
                                         UINT stride, UINT elementCount,
                                         D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.Format = format;
        desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        desc.Buffer.NumElements = elementCount;
        desc.Buffer.StructureByteStride = stride;

        device->CreateUnorderedAccessView(buffer.Get(), nullptr, &desc, destDescriptor);
    }

    void UnorderedAccessView::InitializeWithCounter(Device& device, Buffer& buffer,
                                                    DXGI_FORMAT format, UINT stride,
                                                    UINT elementCount,
                                                    D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.Format = format;
        desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 1;
        desc.Buffer.NumElements = elementCount;
        desc.Buffer.StructureByteStride = stride;

        device->CreateUnorderedAccessView(buffer.Get(), buffer.Get(), &desc, destDescriptor);
    }

} // namespace Boolka