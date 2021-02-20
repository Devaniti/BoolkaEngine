#include "stdafx.h"
#include "ResourceContainer.h"
#include "WindowManagement/DisplayController.h"
#include "RenderSchedule/ResourceTracker.h"

namespace Boolka
{

    bool ResourceContainer::Initialize(Device& device, RenderEngineContext& engineContext, DisplayController& displayController, ResourceTracker& resourceTracker)
    {
        const WindowState& windowState = displayController.GetWindowState();

        UINT width = windowState.width;
        UINT height = windowState.height;

        const DXGI_FORMAT gbufferAlbedoFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        const DXGI_FORMAT gbufferDepthFormat = DXGI_FORMAT_D32_FLOAT;
        const DXGI_FORMAT gbufferDepthSRVFormat = DXGI_FORMAT_R32_FLOAT;
        const size_t rtvHeapDescriptorCount = static_cast<size_t>(RTV::Count) + BLK_IN_FLIGHT_FRAMES;
        const size_t dsvHeapDescriptorCount = static_cast<size_t>(DSV::Count);
        const size_t mainHeapDescriptorCount = static_cast<size_t>(SRV::Count) + static_cast<size_t>(FlipBuf::Count) * BLK_IN_FLIGHT_FRAMES;

        D3D12_CLEAR_VALUE rtvClearValue = {};
        rtvClearValue.Format = gbufferAlbedoFormat;
        rtvClearValue.Color[0] = 0.0f;
        rtvClearValue.Color[1] = 0.0f;
        rtvClearValue.Color[2] = 0.0f;
        rtvClearValue.Color[3] = 0.0f;

        D3D12_CLEAR_VALUE dsvClearValue = {};
        dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        dsvClearValue.DepthStencil.Depth = 1.0f;

        GetTexture(Tex::GBufferAlbedo).Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1,
            gbufferAlbedoFormat, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, &rtvClearValue, D3D12_RESOURCE_STATE_RENDER_TARGET);
        GetTexture(Tex::GBufferNormal).Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1,
            gbufferAlbedoFormat, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, &rtvClearValue, D3D12_RESOURCE_STATE_RENDER_TARGET);
        GetTexture(Tex::LightBuffer).Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1,
            gbufferAlbedoFormat, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, &rtvClearValue, D3D12_RESOURCE_STATE_RENDER_TARGET);
        GetTexture(Tex::GbufferDepth).Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1,
            gbufferDepthFormat, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &dsvClearValue, D3D12_RESOURCE_STATE_DEPTH_WRITE);

        GetDescriptorHeap(DescHeap::RTVHeap).Initialize(device, rtvHeapDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
        GetDescriptorHeap(DescHeap::DSVHeap).Initialize(device, dsvHeapDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
        GetDescriptorHeap(DescHeap::MainHeap).Initialize(device, mainHeapDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

        GetSRV(SRV::GBufferAlbedo).Initialize(device, GetTexture(Tex::GBufferAlbedo), 
            GetDescriptorHeap(DescHeap::MainHeap).GetCPUHandle(static_cast<size_t>(SRV::GBufferAlbedo)));
        GetSRV(SRV::GBufferNormal).Initialize(device, GetTexture(Tex::GBufferNormal),
            GetDescriptorHeap(DescHeap::MainHeap).GetCPUHandle(static_cast<size_t>(SRV::GBufferNormal)));
        GetSRV(SRV::GbufferDepth).Initialize(device, GetTexture(Tex::GbufferDepth),
            GetDescriptorHeap(DescHeap::MainHeap).GetCPUHandle(static_cast<size_t>(SRV::GbufferDepth)), gbufferDepthSRVFormat);
        GetSRV(SRV::LightBuffer).Initialize(device, GetTexture(Tex::LightBuffer),
            GetDescriptorHeap(DescHeap::MainHeap).GetCPUHandle(static_cast<size_t>(SRV::LightBuffer)));

        GetRTV(RTV::GBufferAlbedo).Initialize(device, GetTexture(Tex::GBufferAlbedo), 
            gbufferAlbedoFormat, GetDescriptorHeap(DescHeap::RTVHeap).GetCPUHandle(static_cast<size_t>(RTV::GBufferAlbedo)));
        GetRTV(RTV::GBufferNormal).Initialize(device, GetTexture(Tex::GBufferNormal), 
            gbufferAlbedoFormat, GetDescriptorHeap(DescHeap::RTVHeap).GetCPUHandle(static_cast<size_t>(RTV::GBufferNormal)));
        GetRTV(RTV::LightBuffer).Initialize(device, GetTexture(Tex::LightBuffer),
            gbufferAlbedoFormat, GetDescriptorHeap(DescHeap::RTVHeap).GetCPUHandle(static_cast<size_t>(RTV::LightBuffer)));

        GetDSV(DSV::GbufferDepth).Initialize(device, GetTexture(Tex::GbufferDepth), 
            gbufferDepthFormat, GetDescriptorHeap(DescHeap::DSVHeap).GetCPUHandle(static_cast<size_t>(DSV::GbufferDepth)));

        m_RootSigs[static_cast<size_t>(DSV::GbufferDepth)].Initialize(device, "RootSig.cso");

        static const UINT64 floatSize = 4;
        static const UINT64 frameCbSize = BLK_CEIL_TO_POWER_OF_TWO(sizeof(Matrix4x4) * 6, 256);
        static const UINT64 deferredLightingCbSize = BLK_CEIL_TO_POWER_OF_TWO(sizeof(Vector3) * 4 * 2 + sizeof(unsigned int), 256);

        // Group initializations of same types
        // This can potentially improve performance
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) m_FlippedResources[i].m_BackBuffer = &displayController.GetBuffer(i);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetBackBufferRTV(i).Initialize(device, *m_FlippedResources[i].m_BackBuffer, DXGI_FORMAT_R8G8B8A8_UNORM, 
            GetDescriptorHeap(DescHeap::RTVHeap).GetCPUHandle(static_cast<size_t>(RTV::Count) + i));
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableBuffer(i, FlipBuf::Frame).Initialize(device, frameCbSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableBuffer(i, FlipBuf::DeferredLighting).Initialize(device, deferredLightingCbSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableCBV(i, FlipCBV::Frame).Initialize(device, GetFlippableBuffer(i, FlipBuf::Frame), 
            GetDescriptorHeap(DescHeap::MainHeap).GetCPUHandle(static_cast<size_t>(SRV::Count) + i * static_cast<size_t>(FlipCBV::Count) + static_cast<size_t>(FlipCBV::Frame)), frameCbSize);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableCBV(i, FlipCBV::DeferredLighting).Initialize(device, GetFlippableBuffer(i, FlipBuf::DeferredLighting),
            GetDescriptorHeap(DescHeap::MainHeap).GetCPUHandle(static_cast<size_t>(SRV::Count) + i * static_cast<size_t>(FlipCBV::Count) + static_cast<size_t>(FlipCBV::DeferredLighting)), deferredLightingCbSize);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableUploadBuffer(i, FlipUploadBuf::Frame).Initialize(device, frameCbSize);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableUploadBuffer(i, FlipUploadBuf::DeferredLighting).Initialize(device, deferredLightingCbSize);

        resourceTracker.RegisterResource(GetTexture(Tex::GBufferAlbedo), D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.RegisterResource(GetTexture(Tex::GBufferNormal), D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.RegisterResource(GetTexture(Tex::LightBuffer), D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.RegisterResource(GetTexture(Tex::GbufferDepth), D3D12_RESOURCE_STATE_DEPTH_WRITE);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) resourceTracker.RegisterResource(GetBackBuffer(i), D3D12_RESOURCE_STATE_PRESENT);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) for (size_t j = 0; j < static_cast<size_t>(FlipBuf::Count); ++j)  resourceTracker.RegisterResource(m_FlippedResources[i].m_Buffer[j], D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

#ifdef BLK_RENDER_DEBUG
        GetTexture(Tex::GBufferAlbedo).SetDebugName(L"GBufferAlbedo");
        GetTexture(Tex::GBufferNormal).SetDebugName(L"GBufferNormal");
        GetTexture(Tex::LightBuffer).SetDebugName(L"LightBuffer");
        GetTexture(Tex::GbufferDepth).SetDebugName(L"GbufferDepth");

        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableBuffer(i, FlipBuf::Frame).SetDebugName(L"FrameConstantBuffer%d", i);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableBuffer(i, FlipBuf::DeferredLighting).SetDebugName(L"DeferredLightingConstantBuffer%d", i);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableUploadBuffer(i, FlipUploadBuf::Frame).SetDebugName(L"FrameConstantUploadBuffer%d", i);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i) GetFlippableUploadBuffer(i, FlipUploadBuf::DeferredLighting).SetDebugName(L"DeferredLightingConstantUploadBuffer%d", i);
#endif

        return true;
    }

    void ResourceContainer::Unload()
    {
        BLK_UNLOAD_ARRAY(m_textures);
        //BLK_UNLOAD_ARRAY(m_buffers);
        BLK_UNLOAD_ARRAY(m_descriptorHeaps);
        BLK_UNLOAD_ARRAY(m_SRVs);
        BLK_UNLOAD_ARRAY(m_RTVs);
        BLK_UNLOAD_ARRAY(m_DSVs);
        //BLK_UNLOAD_ARRAY(m_VBVs);
        //BLK_UNLOAD_ARRAY(m_IBVs);
        BLK_UNLOAD_ARRAY(m_RootSigs);

        // When we unloading we group resources by type
        for (auto& flippedResource : m_FlippedResources) flippedResource.m_BackBuffer = nullptr;
        for (auto& flippedResource : m_FlippedResources) flippedResource.m_BackBufferRTV.Unload();
        for (auto& flippedResource : m_FlippedResources) BLK_UNLOAD_ARRAY(flippedResource.m_Buffer);
        for (auto& flippedResource : m_FlippedResources) BLK_UNLOAD_ARRAY(flippedResource.m_ConstantUploadBuffer);
        for (auto& flippedResource : m_FlippedResources) BLK_UNLOAD_ARRAY(flippedResource.m_ConstantBufferView);
    }

}
