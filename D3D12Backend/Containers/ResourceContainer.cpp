#include "stdafx.h"

#include "ResourceContainer.h"

#include "APIWrappers/Device.h"
#include "APIWrappers/RenderDebug.h"
#include "APIWrappers/Resources/Textures/Views/UnorderedAccessView.h"
#include "BoolkaCommon/Structures/Frustum.h"
#include "Containers/Scene.h"
#include "RenderSchedule/ResourceTracker.h"
#include "ShaderTable.h"
#include "WindowManagement/DisplayController.h"

namespace Boolka
{

    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::Tex);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::Buf);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::SRV);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::RTV);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::DSV);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::RootSig);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::DescHeap);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::FlipCBV);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::FlipUploadBuf);
    BLK_DEFINE_ENUM_OPERATORS(ResourceContainer::DefaultRootSigBindPoints);

    bool ResourceContainer::Initialize(Device& device, RenderEngineContext& engineContext,
                                       DisplayController& displayController,
                                       ResourceTracker& resourceTracker)
    {
        BLK_CPU_SCOPE("ResourceContainer::Initialize");

        const WindowState& windowState = displayController.GetWindowState();

        UINT width = windowState.width;
        UINT height = windowState.height;

        const DXGI_FORMAT gbufferAlbedoFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
        const DXGI_FORMAT gbufferDepthFormat = DXGI_FORMAT_D32_FLOAT;
        const DXGI_FORMAT gbufferDepthSRVFormat = DXGI_FORMAT_R32_FLOAT;
        const DXGI_FORMAT shadowMapFormat = DXGI_FORMAT_D32_FLOAT;
        const DXGI_FORMAT shadowMapSRVFormat = DXGI_FORMAT_R32_FLOAT;
        const size_t rtvHeapDescriptorCount =
            static_cast<size_t>(RTV::Count) + BLK_IN_FLIGHT_FRAMES;
        const size_t dsvHeapDescriptorCount = static_cast<size_t>(DSV::Count);
        const size_t mainHeapDescriptorCount =
            static_cast<size_t>(UAV::Count) + static_cast<size_t>(SRV::Count) +
            static_cast<size_t>(CBV::Count) + static_cast<size_t>(Scene::SRVOffset::MaxSize);
        const size_t cpuVisibleDescriptorCount =
            static_cast<size_t>(CPUVisibleDescriptorHeap::Count);

        D3D12_CLEAR_VALUE rtvClearValue = {};
        rtvClearValue.Format = gbufferAlbedoFormat;
        rtvClearValue.Color[0] = 0.0f;
        rtvClearValue.Color[1] = 0.0f;
        rtvClearValue.Color[2] = 0.0f;
        rtvClearValue.Color[3] = 0.0f;

        D3D12_CLEAR_VALUE dsvClearValue = {};
        dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        dsvClearValue.DepthStencil.Depth = 1.0f;

        GetTexture(Tex::GBufferAlbedo)
            .Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1, gbufferAlbedoFormat,
                        D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, &rtvClearValue,
                        D3D12_RESOURCE_STATE_RENDER_TARGET);
        GetTexture(Tex::GBufferNormal)
            .Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1, gbufferAlbedoFormat,
                        D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, &rtvClearValue,
                        D3D12_RESOURCE_STATE_RENDER_TARGET);
        GetTexture(Tex::GBufferRaytraceResults)
            .Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1, gbufferAlbedoFormat,
                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, nullptr,
                        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        GetTexture(Tex::LightBuffer)
            .Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1, gbufferAlbedoFormat,
                        D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, &rtvClearValue,
                        D3D12_RESOURCE_STATE_RENDER_TARGET);
        GetTexture(Tex::GbufferDepth)
            .Initialize(device, D3D12_HEAP_TYPE_DEFAULT, width, height, 1, gbufferDepthFormat,
                        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &dsvClearValue,
                        D3D12_RESOURCE_STATE_DEPTH_WRITE);
        for (size_t i = 0; i < BLK_MAX_LIGHT_COUNT; i++)
        {
            GetTexture(Tex::ShadowMapCube0 + i)
                .Initialize(device, D3D12_HEAP_TYPE_DEFAULT, BLK_LIGHT_SHADOWMAP_SIZE,
                            BLK_LIGHT_SHADOWMAP_SIZE, 1, shadowMapFormat,
                            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &dsvClearValue,
                            D3D12_RESOURCE_STATE_DEPTH_WRITE, 6);
        }
        GetTexture(Tex::ShadowMapSun)
            .Initialize(device, D3D12_HEAP_TYPE_DEFAULT, BLK_SUN_SHADOWMAP_SIZE,
                        BLK_SUN_SHADOWMAP_SIZE, 1, shadowMapFormat,
                        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &dsvClearValue,
                        D3D12_RESOURCE_STATE_DEPTH_WRITE);

        GetDescriptorHeap(DescHeap::RTVHeap)
            .Initialize(device, rtvHeapDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
                        D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
        GetDescriptorHeap(DescHeap::DSVHeap)
            .Initialize(device, dsvHeapDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
                        D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
        GetDescriptorHeap(DescHeap::MainHeap)
            .Initialize(device, mainHeapDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        GetDescriptorHeap(DescHeap::MainCPUVisibleHeap)
            .Initialize(device, cpuVisibleDescriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                        D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

        m_RootSigs[static_cast<size_t>(DSV::GbufferDepth)].Initialize(device, "RootSig.cso");

        static const UINT64 frameCbSize =
            BLK_CEIL_TO_POWER_OF_TWO(sizeof(HLSLShared::FrameConstantBuffer), 256);
        static const UINT64 deferredLightingCbSize = BLK_CEIL_TO_POWER_OF_TWO(
            sizeof(Vector4) * BLK_TEXCUBE_FACE_COUNT * 2 + sizeof(Vector4u), 256);
        static const UINT gpuCullingBufElements =
            BLK_RENDER_VIEW_COUNT * (Scene::MaxObjectCount + 1) *
            2; // One for each object and additional one for counter
        static const UINT64 gpuCullingBufSize =
            BLK_CEIL_TO_POWER_OF_TWO(sizeof(uint) * gpuCullingBufElements, 256);
        static const UINT gpuCullingDebugReadbackBufElements = BLK_RENDER_VIEW_COUNT * 2;
        static const UINT64 gpuCullingDebugReadbackBufSize =
            BLK_CEIL_TO_POWER_OF_TWO(sizeof(uint) * gpuCullingDebugReadbackBufElements, 256);
        static const UINT64 gpuCullingCBSize =
            BLK_CEIL_TO_POWER_OF_TWO(sizeof(HLSLShared::CullingDataConstantBuffer), 256);
        static const UINT gpuCullingCommandBufElements =
            BLK_RENDER_VIEW_COUNT * BLK_INT_DIVIDE_CEIL(Scene::MaxObjectCount, 32);
        static const UINT gpuCullingCommandBufSize =
            BLK_CEIL_TO_POWER_OF_TWO(gpuCullingCommandBufElements * 32, 256);
        static const UINT gpuCullingMeshletIndicesBufElements =
            Scene::MaxMeshlets * BLK_RENDER_VIEW_COUNT;
        static const UINT gpuCullingMeshletIndicesBufSize =
            BLK_CEIL_TO_POWER_OF_TWO(gpuCullingMeshletIndicesBufElements * sizeof(uint), 256);
        static const UINT64 rtShaderTableBufSize =
            ShaderTable::CalculateRequiredBufferSize(1, 1, 1);
        static const UINT debugMarkersBufElements = 256;
        static const UINT debugMarkersBufSize =
            BLK_CEIL_TO_POWER_OF_TWO(debugMarkersBufElements * sizeof(uint), 256);

        // Group initializations of same types
        // This can potentially improve performance
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            m_FlippedResources[i].m_BackBuffer = &displayController.GetBuffer(i);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            GetBackBufferRTV(i).Initialize(device, *m_FlippedResources[i].m_BackBuffer,
                                           DXGI_FORMAT_R8G8B8A8_UNORM,
                                           GetDescriptorHeap(DescHeap::RTVHeap)
                                               .GetCPUHandle(static_cast<size_t>(RTV::Count) + i));
        GetBuffer(Buf::Frame)
            .Initialize(device, frameCbSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_NONE,
                        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        GetBuffer(Buf::DeferredLighting)
            .Initialize(device, deferredLightingCbSize, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        GetBuffer(Buf::GPUCullingCB)
            .Initialize(device, gpuCullingCBSize, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        GetBuffer(Buf::GPUCulling)
            .Initialize(device, gpuCullingBufSize, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        GetBuffer(Buf::GPUCullingDebugReadback)
            .Initialize(device, gpuCullingDebugReadbackBufSize, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        GetBuffer(Buf::GPUCullingCommand)
            .Initialize(device, gpuCullingCommandBufSize, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        GetBuffer(Buf::GPUCullingMeshletIndices)
            .Initialize(device, gpuCullingMeshletIndicesBufSize, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        GetBuffer(Buf::RTShaderTable)
            .Initialize(device, rtShaderTableBufSize, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
        GetBuffer(Buf::DebugMarkers)
            .Initialize(device, debugMarkersBufSize, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        const UINT uavOffset = static_cast<UINT>(MainSRVDescriptorHeapOffsets::UAVHeapOffset);

        UnorderedAccessView::Initialize(
            device, GetTexture(Tex::GBufferRaytraceResults), DXGI_FORMAT_R16G16B16A16_FLOAT,
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(uavOffset + static_cast<UINT>(SRV::GBufferAlbedo)));

        const UINT srvOffset = static_cast<UINT>(MainSRVDescriptorHeapOffsets::SRVHeapOffset);

        ShaderResourceView::Initialize(
            device, GetTexture(Tex::GBufferAlbedo),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(srvOffset + static_cast<UINT>(SRV::GBufferAlbedo)));
        ShaderResourceView::Initialize(
            device, GetTexture(Tex::GBufferNormal),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(srvOffset + static_cast<UINT>(SRV::GBufferNormal)));
        ShaderResourceView::Initialize(
            device, GetTexture(Tex::GBufferRaytraceResults),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(srvOffset + static_cast<UINT>(SRV::GBufferRaytraceResults)));
        ShaderResourceView::Initialize(
            device, GetTexture(Tex::GbufferDepth),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(srvOffset + static_cast<UINT>(SRV::GbufferDepth)),
            gbufferDepthSRVFormat);
        ShaderResourceView::Initialize(
            device, GetTexture(Tex::LightBuffer),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(srvOffset + static_cast<UINT>(SRV::LightBuffer)));
        ShaderResourceView::Initialize(
            device, GetBuffer(Buf::GPUCulling), gpuCullingBufElements, DXGI_FORMAT_R32_UINT,
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(srvOffset + static_cast<UINT>(SRV::GPUCulling)));
        for (UINT i = 0; i < BLK_MAX_LIGHT_COUNT; i++)
        {
            ShaderResourceView::InitializeCube(
                device, GetTexture(Tex::ShadowMapCube0 + i),
                GetDescriptorHeap(DescHeap::MainHeap)
                    .GetCPUHandle(srvOffset + static_cast<UINT>(SRV::ShadowMapCube0 + i)),
                shadowMapSRVFormat);
        }
        ShaderResourceView::Initialize(
            device, GetTexture(Tex::ShadowMapSun),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(srvOffset + static_cast<size_t>(SRV::ShadowMapSun)),
            shadowMapSRVFormat);

        GetRTV(RTV::GBufferAlbedo)
            .Initialize(device, GetTexture(Tex::GBufferAlbedo), gbufferAlbedoFormat,
                        GetDescriptorHeap(DescHeap::RTVHeap)
                            .GetCPUHandle(static_cast<size_t>(RTV::GBufferAlbedo)));
        GetRTV(RTV::GBufferNormal)
            .Initialize(device, GetTexture(Tex::GBufferNormal), gbufferAlbedoFormat,
                        GetDescriptorHeap(DescHeap::RTVHeap)
                            .GetCPUHandle(static_cast<size_t>(RTV::GBufferNormal)));
        GetRTV(RTV::LightBuffer)
            .Initialize(device, GetTexture(Tex::LightBuffer), gbufferAlbedoFormat,
                        GetDescriptorHeap(DescHeap::RTVHeap)
                            .GetCPUHandle(static_cast<size_t>(RTV::LightBuffer)));

        GetDSV(DSV::GbufferDepth)
            .Initialize(device, GetTexture(Tex::GbufferDepth), gbufferDepthFormat,
                        GetDescriptorHeap(DescHeap::DSVHeap)
                            .GetCPUHandle(static_cast<size_t>(DSV::GbufferDepth)));
        for (UINT textureIndex = 0; textureIndex < BLK_MAX_LIGHT_COUNT; textureIndex++)
        {
            for (UINT16 arraySlice = 0; arraySlice < BLK_TEXCUBE_FACE_COUNT; ++arraySlice)
            {
                GetDSV(DSV::ShadowMapLight0 + textureIndex * BLK_TEXCUBE_FACE_COUNT + arraySlice)
                    .Initialize(device, GetTexture(Tex::ShadowMapCube0 + textureIndex),
                                shadowMapFormat,
                                GetDescriptorHeap(DescHeap::DSVHeap)
                                    .GetCPUHandle(static_cast<size_t>(
                                        DSV::ShadowMapLight0 + textureIndex * 6 + arraySlice)),
                                arraySlice);
            }
        }
        GetDSV(DSV::ShadowMapSun)
            .Initialize(device, GetTexture(Tex::ShadowMapSun), gbufferDepthFormat,
                        GetDescriptorHeap(DescHeap::DSVHeap)
                            .GetCPUHandle(static_cast<size_t>(DSV::ShadowMapSun)));

        ConstantBufferView::Initialize(
            device, GetBuffer(Buf::Frame),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(static_cast<size_t>(MainSRVDescriptorHeapOffsets::CBVHeapOffset) +
                              static_cast<size_t>(CBV::Frame)),
            frameCbSize);
        ConstantBufferView::Initialize(
            device, GetBuffer(Buf::DeferredLighting),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(static_cast<size_t>(MainSRVDescriptorHeapOffsets::CBVHeapOffset) +
                              static_cast<size_t>(CBV::DeferredLighting)),
            deferredLightingCbSize);
        UnorderedAccessView::Initialize(device, GetBuffer(Buf::GPUCulling), DXGI_FORMAT_R32_UINT,
                                        gpuCullingBufElements,
                                        GetCPUVisibleCPUDescriptor(Buf::GPUCulling));
        UnorderedAccessView::Initialize(device, GetBuffer(Buf::GPUCullingDebugReadback),
                                        DXGI_FORMAT_R32_UINT, gpuCullingDebugReadbackBufElements,
                                        GetCPUDescriptor(Buf::GPUCullingDebugReadback));
        UnorderedAccessView::Initialize(device, GetBuffer(Buf::GPUCullingCommand), 32,
                                        gpuCullingCommandBufElements,
                                        GetCPUDescriptor(Buf::GPUCullingCommand));
        UnorderedAccessView::Initialize(device, GetBuffer(Buf::GPUCullingCommand),
                                        DXGI_FORMAT_R32_UINT, gpuCullingCommandBufElements,
                                        GetCPUVisibleCPUDescriptor(Buf::GPUCullingCommand));
        UnorderedAccessView::Initialize(device, GetBuffer(Buf::GPUCullingMeshletIndices),
                                        DXGI_FORMAT_R32_UINT, gpuCullingMeshletIndicesBufElements,
                                        GetCPUDescriptor(Buf::GPUCullingMeshletIndices));
        UnorderedAccessView::Initialize(device, GetBuffer(Buf::DebugMarkers), DXGI_FORMAT_R32_UINT,
                                        debugMarkersBufElements,
                                        GetCPUVisibleCPUDescriptor(Buf::DebugMarkers));
        ConstantBufferView::Initialize(
            device, GetBuffer(Buf::GPUCullingCB),
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(static_cast<size_t>(MainSRVDescriptorHeapOffsets::CBVHeapOffset) +
                              static_cast<size_t>(CBV::GPUCulling)),
            gpuCullingCBSize);
        device->CopyDescriptorsSimple(1, GetCPUDescriptor(Buf::GPUCulling),
                                      GetCPUVisibleCPUDescriptor(Buf::GPUCulling),
                                      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        device->CopyDescriptorsSimple(
            1,
            GetDescriptorHeap(DescHeap::MainHeap)
                .GetCPUHandle(static_cast<size_t>(MainSRVDescriptorHeapOffsets::UAVHeapOffset) +
                              static_cast<size_t>(UAV::GPUCullingCommandUINT)),
            GetCPUVisibleCPUDescriptor(Buf::GPUCullingCommand),
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        device->CopyDescriptorsSimple(1, GetCPUDescriptor(Buf::DebugMarkers),
                                      GetCPUVisibleCPUDescriptor(Buf::DebugMarkers),
                                      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            GetFlippableUploadBuffer(i, FlipUploadBuf::Frame).Initialize(device, frameCbSize);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            GetFlippableUploadBuffer(i, FlipUploadBuf::DeferredLighting)
                .Initialize(device, deferredLightingCbSize);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            GetFlippableUploadBuffer(i, FlipUploadBuf::GPUCulling)
                .Initialize(device, gpuCullingCBSize);

        resourceTracker.RegisterResource(GetTexture(Tex::GBufferAlbedo),
                                         D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.RegisterResource(GetTexture(Tex::GBufferNormal),
                                         D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.RegisterResource(GetTexture(Tex::GBufferRaytraceResults),
                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        resourceTracker.RegisterResource(GetTexture(Tex::LightBuffer),
                                         D3D12_RESOURCE_STATE_RENDER_TARGET);
        resourceTracker.RegisterResource(GetTexture(Tex::GbufferDepth),
                                         D3D12_RESOURCE_STATE_DEPTH_WRITE);
        for (size_t i = 0; i < BLK_MAX_LIGHT_COUNT; i++)
            resourceTracker.RegisterResource(GetTexture(Tex::ShadowMapCube0 + i),
                                             D3D12_RESOURCE_STATE_DEPTH_WRITE);
        resourceTracker.RegisterResource(GetTexture(Tex::ShadowMapSun),
                                         D3D12_RESOURCE_STATE_DEPTH_WRITE);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            resourceTracker.RegisterResource(GetBackBuffer(i), D3D12_RESOURCE_STATE_PRESENT);
        resourceTracker.RegisterResource(GetBuffer(Buf::Frame),
                                         D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        resourceTracker.RegisterResource(GetBuffer(Buf::DeferredLighting),
                                         D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        resourceTracker.RegisterResource(GetBuffer(Buf::GPUCullingCB),
                                         D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        resourceTracker.RegisterResource(GetBuffer(Buf::GPUCulling),
                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        resourceTracker.RegisterResource(GetBuffer(Buf::GPUCullingDebugReadback),
                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        resourceTracker.RegisterResource(GetBuffer(Buf::GPUCullingCommand),
                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        resourceTracker.RegisterResource(GetBuffer(Buf::GPUCullingMeshletIndices),
                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        resourceTracker.RegisterResource(GetBuffer(Buf::DebugMarkers),
                                         D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

#ifdef BLK_RENDER_DEBUG
        RenderDebug::SetDebugName(GetTexture(Tex::GBufferAlbedo).Get(), L"GBufferAlbedo");
        RenderDebug::SetDebugName(GetTexture(Tex::GBufferNormal).Get(), L"GBufferNormal");
        RenderDebug::SetDebugName(GetTexture(Tex::GBufferRaytraceResults).Get(),
                                  L"GBufferRaytraceResults");
        RenderDebug::SetDebugName(GetTexture(Tex::LightBuffer).Get(), L"LightBuffer");
        RenderDebug::SetDebugName(GetTexture(Tex::GbufferDepth).Get(), L"GbufferDepth");
        for (size_t i = 0; i < BLK_MAX_LIGHT_COUNT; i++)
            RenderDebug::SetDebugName(GetTexture(Tex::ShadowMapCube0 + i).Get(), L"ShadowMapCube%d",
                                      i);
        RenderDebug::SetDebugName(GetTexture(Tex::ShadowMapSun).Get(), L"ShadowMapSun");

        RenderDebug::SetDebugName(GetBuffer(Buf::Frame).Get(), L"FrameConstantBuffer");
        RenderDebug::SetDebugName(GetBuffer(Buf::DeferredLighting).Get(),
                                  L"DeferredLightingConstantBuffer");
        RenderDebug::SetDebugName(GetBuffer(Buf::GPUCullingCB).Get(), L"GPUCullingCBBuffer");
        RenderDebug::SetDebugName(GetBuffer(Buf::GPUCulling).Get(), L"GPUCullingBuffer");
        RenderDebug::SetDebugName(GetBuffer(Buf::GPUCullingDebugReadback).Get(),
                                  L"GPUCullingDebugReadbackBuffer");
        RenderDebug::SetDebugName(GetBuffer(Buf::GPUCullingCommand).Get(), L"GPUCullingCommand");
        RenderDebug::SetDebugName(GetBuffer(Buf::GPUCullingMeshletIndices).Get(),
                                  L"GPUCullingMeshletIndices");
        RenderDebug::SetDebugName(GetBuffer(Buf::RTShaderTable).Get(), L"RTShaderTable");
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            RenderDebug::SetDebugName(GetFlippableUploadBuffer(i, FlipUploadBuf::Frame).Get(),
                                      L"FrameConstantUploadBuffer%d", i);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            RenderDebug::SetDebugName(
                GetFlippableUploadBuffer(i, FlipUploadBuf::DeferredLighting).Get(),
                L"DeferredLightingConstantUploadBuffer%d", i);
        for (UINT i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
            RenderDebug::SetDebugName(GetFlippableUploadBuffer(i, FlipUploadBuf::GPUCulling).Get(),
                                      L"GPUCullingConstantUploadBuffer%d", i);
#endif

        return true;
    }

    void ResourceContainer::Unload()
    {
        BLK_UNLOAD_ARRAY(m_Textures);
        BLK_UNLOAD_ARRAY(m_Buffers);
        BLK_UNLOAD_ARRAY(m_DescriptorHeaps);
        BLK_UNLOAD_ARRAY(m_RTVs);
        BLK_UNLOAD_ARRAY(m_DSVs);
        BLK_UNLOAD_ARRAY(m_RootSigs);

        // When we unloading we group resources by type
        for (auto& flippedResource : m_FlippedResources)
            flippedResource.m_BackBuffer = nullptr;
        for (auto& flippedResource : m_FlippedResources)
            flippedResource.m_BackBufferRTV.Unload();
        for (auto& flippedResource : m_FlippedResources)
            BLK_UNLOAD_ARRAY(flippedResource.m_ConstantUploadBuffer);
    }

    Texture2D& ResourceContainer::GetTexture(Tex id)
    {
        return m_Textures[static_cast<size_t>(id)];
    }

    Buffer& ResourceContainer::GetBuffer(Buf id)
    {
        return m_Buffers[static_cast<size_t>(id)];
    }

    DescriptorHeap& ResourceContainer::GetDescriptorHeap(DescHeap id)
    {
        return m_DescriptorHeaps[static_cast<size_t>(id)];
    }

    RenderTargetView& ResourceContainer::GetRTV(RTV id)
    {
        return m_RTVs[static_cast<size_t>(id)];
    }

    DepthStencilView& ResourceContainer::GetDSV(DSV id)
    {
        return m_DSVs[static_cast<size_t>(id)];
    }

    RootSignature& ResourceContainer::GetRootSignature(RootSig id)
    {
        return m_RootSigs[static_cast<size_t>(id)];
    }

    Texture2D& ResourceContainer::GetBackBuffer(UINT frameIndex)
    {
        BLK_ASSERT(m_FlippedResources[frameIndex].m_BackBuffer != nullptr);
        return *m_FlippedResources[frameIndex].m_BackBuffer;
    }

    RenderTargetView& ResourceContainer::GetBackBufferRTV(UINT frameIndex)
    {
        return m_FlippedResources[frameIndex].m_BackBufferRTV;
    }

    UploadBuffer& ResourceContainer::GetFlippableUploadBuffer(UINT frameIndex, FlipUploadBuf id)
    {
        return m_FlippedResources[frameIndex].m_ConstantUploadBuffer[static_cast<size_t>(id)];
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ResourceContainer::GetGPUDescriptor(Buf id)
    {
        return GetDescriptorHeap(DescHeap::MainHeap).GetGPUHandle(GetDescriptorHeapOffset(id));
    }

    D3D12_CPU_DESCRIPTOR_HANDLE ResourceContainer::GetCPUDescriptor(Buf id)
    {
        return GetDescriptorHeap(DescHeap::MainHeap).GetCPUHandle(GetDescriptorHeapOffset(id));
    }

    D3D12_CPU_DESCRIPTOR_HANDLE ResourceContainer::GetCPUVisibleCPUDescriptor(Buf id)
    {
        switch (id)
        {
        case Buf::GPUCulling:
            return GetDescriptorHeap(DescHeap::MainCPUVisibleHeap)
                .GetCPUHandle(static_cast<UINT>(CPUVisibleDescriptorHeap::GPUCulling));
            break;
        case Buf::GPUCullingCommand:
            return GetDescriptorHeap(DescHeap::MainCPUVisibleHeap)
                .GetCPUHandle(static_cast<UINT>(CPUVisibleDescriptorHeap::GPUCullingCommandUINT));
            break;
        case Buf::DebugMarkers:
            return GetDescriptorHeap(DescHeap::MainCPUVisibleHeap)
                .GetCPUHandle(static_cast<UINT>(CPUVisibleDescriptorHeap::DebugMarkers));
            break;
        default:
            BLK_ASSERT(0);
            return D3D12_CPU_DESCRIPTOR_HANDLE();
        }
    }

    UINT ResourceContainer::GetDescriptorHeapOffset(Buf id)
    {
        switch (id)
        {
        case Buf::Frame:
            return static_cast<UINT>(MainSRVDescriptorHeapOffsets::CBVHeapOffset) +
                   static_cast<UINT>(CBV::Frame);
            break;
        case Buf::DeferredLighting:
            return static_cast<UINT>(MainSRVDescriptorHeapOffsets::CBVHeapOffset) +
                   static_cast<UINT>(CBV::DeferredLighting);
            break;
        case Buf::GPUCullingCB:
            return static_cast<UINT>(MainSRVDescriptorHeapOffsets::CBVHeapOffset) +
                   static_cast<UINT>(CBV::GPUCulling);
            break;
        case Buf::GPUCulling:
            return static_cast<UINT>(MainSRVDescriptorHeapOffsets::UAVHeapOffset) +
                   static_cast<UINT>(UAV::GPUCulling);
            break;
        case Buf::GPUCullingDebugReadback:
            return static_cast<UINT>(MainSRVDescriptorHeapOffsets::UAVHeapOffset) +
                   static_cast<UINT>(UAV::GPUCullingDebugReadback);
            break;
        case Buf::GPUCullingCommand:
            return static_cast<UINT>(MainSRVDescriptorHeapOffsets::UAVHeapOffset) +
                   static_cast<UINT>(UAV::GPUCullingCommand);
            break;
        case Buf::GPUCullingMeshletIndices:
            return static_cast<UINT>(MainSRVDescriptorHeapOffsets::UAVHeapOffset) +
                   static_cast<UINT>(UAV::GPUCullingMeshletIndices);
            break;
        case Buf::DebugMarkers:
            return static_cast<UINT>(MainSRVDescriptorHeapOffsets::UAVHeapOffset) +
                   static_cast<UINT>(UAV::DebugMarkers);
            break;
        default:
            BLK_ASSERT(0);
            return 0;
            break;
        }
    }

} // namespace Boolka
