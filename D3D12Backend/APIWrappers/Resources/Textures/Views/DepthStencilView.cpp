#include "stdafx.h"
#include "DepthStencilView.h"
#include "APIWrappers/Device.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"

namespace Boolka
{

    DepthStencilView::DepthStencilView()
        : m_CPUDescriptorHandle{}
    {
    }

    DepthStencilView::~DepthStencilView()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);
    }

    bool DepthStencilView::Initialize(Device& device, Texture2D& texture, DXGI_FORMAT format, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr == 0);
        
        D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
        desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        desc.Format = format;

        device->CreateDepthStencilView(texture.Get(), &desc, destDescriptor);
        m_CPUDescriptorHandle = destDescriptor;

        return true;
    }

    void DepthStencilView::Unload()
    {
        BLK_ASSERT(m_CPUDescriptorHandle.ptr != 0);
        m_CPUDescriptorHandle = {};
    }
}