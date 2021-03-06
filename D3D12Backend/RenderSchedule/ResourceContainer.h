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

    class ResourceContainer
    {
    public:
        enum class Tex
        {
            GBufferAlbedo,
            GBufferNormal,
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

        enum class FlipCBV
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
            FrameConstantBuffer = 0,
            PassConstantBuffer = 1,
            PassRootConstant = 2,
            RenderPassSRV = 3,
            SceneSRV = 5
        };

        ResourceContainer() = default;
        ~ResourceContainer() = default;

        bool Initialize(Device& device, RenderEngineContext& engineContext,
                        DisplayController& displayController, ResourceTracker& resourceTracker);
        void Unload();

        Texture2D& GetTexture(Tex id);
        DescriptorHeap& GetDescriptorHeap(DescHeap id);
        ShaderResourceView& GetSRV(SRV id);
        RenderTargetView& GetRTV(RTV id);
        DepthStencilView& GetDSV(DSV id);
        RootSignature& GetRootSignature(RootSig id);

        Texture2D& GetBackBuffer(UINT frameIndex);
        RenderTargetView& GetBackBufferRTV(UINT frameIndex);
        Buffer& GetFlippableBuffer(UINT frameIndex, FlipBuf id);
        ConstantBufferView& GetFlippableCBV(UINT frameIndex, FlipCBV id);
        UploadBuffer& GetFlippableUploadBuffer(UINT frameIndex, FlipUploadBuf id);

    private:
        Texture2D m_textures[static_cast<size_t>(Tex::Count)];
        DescriptorHeap m_descriptorHeaps[static_cast<size_t>(DescHeap::Count)];
        ShaderResourceView m_SRVs[static_cast<size_t>(SRV::Count)];
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
            ConstantBufferView m_ConstantBufferView[static_cast<size_t>(FlipCBV::Count)];
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
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::FlipCBV);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::FlipUploadBuf);
    BLK_DECLARE_ENUM_OPERATORS(ResourceContainer::DefaultRootSigBindPoints);

} // namespace Boolka
