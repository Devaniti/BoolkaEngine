#include "stdafx.h"

#include "UnorderedAccessView.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"

namespace Boolka
{

    void UnorderedAccessView::Initialize(Device& device, Texture2D& texture, DXGI_FORMAT format,
                                         D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.Format = format;
        desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        device->CreateUnorderedAccessView(texture.Get(), nullptr, &desc, destDescriptor);
    }

} // namespace Boolka