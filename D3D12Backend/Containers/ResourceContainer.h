#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/ConstantBufferView.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "APIWrappers/Resources/Textures/Views/ShaderResourceView.h"
#include "APIWrappers/RootSignature.h"

namespace Boolka
{

    class Device;
    class RenderContext;
    class DisplayController;
    class ResourceTracker;
    class RenderEngineContext;

    class ResourceContainer
    {
    public:
        enum class Tex
        {
            GBufferAlbedo,
            GBufferNormal,
            GBufferReflections,
            GbufferDepth,
            LightBuffer,
            ShadowMapCube0,
            ShadowMapSun = ShadowMapCube0 + BLK_MAX_LIGHT_COUNT,
            Count
        };

        enum class SRV
        {
            GBufferAlbedo,
            GBufferNormal,
            GBufferReflections,
            GbufferDepth,
            LightBuffer,
            ShadowMapCube0,
            ShadowMapSun = ShadowMapCube0 + BLK_MAX_LIGHT_COUNT,
            Count
        };

        enum class RTV
        {
            GBufferAlbedo,
            GBufferNormal,
            LightBuffer,
            Count
        };

        enum class UAV
        {
            GBufferReflections,
            Count
        };

        enum class DSV
        {
            GbufferDepth,
            ShadowMapLight0,
            ShadowMapSun = ShadowMapLight0 + BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT,
            Count
        };

        enum class RootSig
        {
            Default,
            Count
        };

        enum class DescHeap
        {
            RTVHeap,
            DSVHeap,
            MainHeap,
            Count
        };

        enum class FlipBuf
        {
            Frame,
            DeferredLighting,
            ShadowMap,
            Count
        };

        enum class FlipSRV
        {
            Frame,
            Count
        };

        enum class FlipUploadBuf
        {
            Frame,
            DeferredLighting,
            ShadowMap,
            Count
        };

        enum class DefaultRootSigBindPoints
        {
            FrameConstantBuffer,
            PassConstantBuffer,
            PassRootConstant,
            RenderPassSRV,
            SceneTexturesSRV,
            SceneResourcesSRV
        };

        enum class MainSRVDescriptorHeapOffsets
        {
            UAVHeapOffset = 0,
            SRVHeapOffset = UAVHeapOffset + 1,
            FlipCBVHeapOffset = SRVHeapOffset + static_cast<int>(SRV::Count),
            SceneSRVHeapOffset =
                FlipCBVHeapOffset + static_cast<int>(FlipSRV::Count) * BLK_IN_FLIGHT_FRAMES
        };

        ResourceContainer() = default;
        ~ResourceContainer() = default;

        bool Initialize(Device& device, RenderEngineContext& engineContext,
                        DisplayController& displayController, ResourceTracker& resourceTracker);
        void Unload();

        Texture2D& GetTexture(Tex id);
        DescriptorHeap& GetDescriptorHeap(DescHeap id);
        RenderTargetView& GetRTV(RTV id);
        DepthStencilView& GetDSV(DSV id);
        RootSignature& GetRootSignature(RootSig id);

        Texture2D& GetBackBuffer(UINT frameIndex);
        RenderTargetView& GetBackBufferRTV(UINT frameIndex);
        Buffer& GetFlippableBuffer(UINT frameIndex, FlipBuf id);
        ConstantBufferView& GetFlippableCBV(UINT frameIndex, FlipSRV id);
        UploadBuffer& GetFlippableUploadBuffer(UINT frameIndex, FlipUploadBuf id);

    private:
        Texture2D m_textures[static_cast<size_t>(Tex::Count)];
        DescriptorHeap m_descriptorHeaps[static_cast<size_t>(DescHeap::Count)];
        RenderTargetView m_RTVs[static_cast<size_t>(RTV::Count)];
        DepthStencilView m_DSVs[static_cast<size_t>(DSV::Count)];
        RootSignature m_RootSigs[static_cast<size_t>(RootSig::Count)];

        struct FlippedResources
        {
            // All backbuffer references managed inside swapchain
            // Can't have texture here, since resizing swapchain requires
            // releasing all references
            Texture2D* m_BackBuffer;
            RenderTargetView m_BackBufferRTV;
            Buffer m_Buffer[static_cast<size_t>(FlipBuf::Count)];
            ConstantBufferView m_ConstantBufferView[static_cast<size_t>(FlipSRV::Count)];
            UploadBuffer m_ConstantUploadBuffer[static_cast<size_t>(FlipUploadBuf::Count)];

            FlippedResources()
                : m_BackBuffer(nullptr){};
        };

        FlippedResources m_FlippedResources[BLK_IN_FLIGHT_FRAMES];
    };

    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::Tex);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::SRV);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::RTV);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::DSV);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::RootSig);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::DescHeap);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::FlipBuf);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::FlipSRV);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::FlipUploadBuf);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::DefaultRootSigBindPoints);

} // namespace Boolka
