#include "stdafx.h"
#include "Texture2D.h"
#include "APIWrappers/Device.h"

namespace Boolka
{

    Texture2D::Texture2D()
    {
    }

    Texture2D::~Texture2D()
    {
    }

    bool Texture2D::Initialize(ID3D12Resource* resource)
    {
        BLK_ASSERT(m_Resource == nullptr);
#ifdef BLK_DEBUG
        D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();
        BLK_ASSERT(resourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D);
#endif

        m_Resource = resource;
        //m_Resource->AddRef();

        return true;
    }

    bool Texture2D::Initialize(Device& device, D3D12_HEAP_TYPE heapType, UINT64 width, UINT height, UINT16 mipCount, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags, D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES initialState)
    {
        BLK_ASSERT(m_Resource == nullptr);

        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = heapType;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = width;
        resourceDesc.Height = height;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = mipCount;
        resourceDesc.Format = format;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = resourceFlags;

        HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, initialState, clearValue, IID_PPV_ARGS(&m_Resource));
        return SUCCEEDED(hr) && (m_Resource != nullptr);
    }

    void Texture2D::Unload()
    {
        BLK_ASSERT(m_Resource);

        m_Resource->Release();
        m_Resource = nullptr;
    }

}