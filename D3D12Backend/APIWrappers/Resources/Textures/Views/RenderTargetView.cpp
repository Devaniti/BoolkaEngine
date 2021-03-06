#include "stdafx.h"

#include "RenderTargetView.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"

namespace Boolka
{

    RenderTargetView::RenderTargetView()
        : m_CPUDescriptorHandle{}
    {
    }

    RenderTargetView::~RenderTargetView()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);
    }

    bool RenderTargetView::Initialize(Device& device, Texture2D& texture, DXGI_FORMAT format,
                                      D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);

        D3D12_RENDER_TARGET_VIEW_DESC desc = {};
        desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        desc.Format = format;

        device->CreateRenderTargetView(texture.Get(), &desc, destDescriptor);
        m_CPUDescriptorHandle = destDescriptor;

        return true;
    }

    void RenderTargetView::Unload()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0);
        m_CPUDescriptorHandle = {};
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* RenderTargetView::GetCPUDescriptor()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0);
        return &m_CPUDescriptorHandle;
    }

} // namespace Boolka