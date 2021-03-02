#pragma once
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/RenderTargetView.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/Resources/Buffers/Buffer.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/ConstantBufferView.h"
#include "APIWrappers/RootSignature.h"
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Resources/Textures/Views/ShaderResourceView.h"

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

        enum class Buf
        {
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

        enum class VBV
        {
            Count
        };

        enum class IBV
        {
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

        bool Initialize(Device& device, RenderEngineContext& engineContext, DisplayController& displayController, ResourceTracker& resourceTracker);
        void Unload();

        Texture2D& GetTexture(Tex id) { return m_textures[static_cast<size_t>(id)]; }
        //Buffer& GetBuffer(Buf id) { return m_buffers[static_cast<size_t>(id)]; }
        DescriptorHeap& GetDescriptorHeap(DescHeap id) { return m_descriptorHeaps[static_cast<size_t>(id)]; }
        ShaderResourceView& GetSRV(SRV id) { return m_SRVs[static_cast<size_t>(id)]; }
        RenderTargetView& GetRTV(RTV id) { return m_RTVs[static_cast<size_t>(id)]; }
        DepthStencilView& GetDSV(DSV id) { return m_DSVs[static_cast<size_t>(id)]; }
        //VertexBufferView& GetVBV(VBV id) { return m_VBVs[static_cast<size_t>(id)]; }
        //IndexBufferView& GetIBV(IBV id) { return m_IBVs[static_cast<size_t>(id)]; }
        RootSignature& GetRootSignature(RootSig id) { return m_RootSigs[static_cast<size_t>(id)]; }

        Texture2D& GetBackBuffer(UINT frameIndex) { BLK_ASSERT(m_FlippedResources[frameIndex].m_BackBuffer != nullptr); return *m_FlippedResources[frameIndex].m_BackBuffer; }
        RenderTargetView& GetBackBufferRTV(UINT frameIndex) { return m_FlippedResources[frameIndex].m_BackBufferRTV; }
        Buffer& GetFlippableBuffer(UINT frameIndex, FlipBuf id) { return m_FlippedResources[frameIndex].m_Buffer[static_cast<size_t>(id)]; }
        ConstantBufferView& GetFlippableCBV(UINT frameIndex, FlipCBV id) { return m_FlippedResources[frameIndex].m_ConstantBufferView[static_cast<size_t>(id)]; }
        UploadBuffer& GetFlippableUploadBuffer(UINT frameIndex, FlipUploadBuf id) { return m_FlippedResources[frameIndex].m_ConstantUploadBuffer[static_cast<size_t>(id)]; }
    private:

        Texture2D m_textures[static_cast<size_t>(Tex::Count)];
        //Buffer m_buffers[static_cast<size_t>(Buf::Count)];
        DescriptorHeap m_descriptorHeaps[static_cast<size_t>(DescHeap::Count)];
        ShaderResourceView m_SRVs[static_cast<size_t>(SRV::Count)];
        RenderTargetView m_RTVs[static_cast<size_t>(RTV::Count)];
        DepthStencilView m_DSVs[static_cast<size_t>(DSV::Count)];
        //VertexBufferView m_VBVs[static_cast<size_t>(VBV::Count)];
        //IndexBufferView m_IBVs[static_cast<size_t>(IBV::Count)];
        RootSignature m_RootSigs[static_cast<size_t>(RootSig::Count)];

        struct FlippedResources
        {
            // All backbuffer references managed inside swapchain
            // Can't have texture here, since resizing swapchain requires releasing all references
            Texture2D* m_BackBuffer;
            RenderTargetView m_BackBufferRTV;
            Buffer m_Buffer[static_cast<size_t>(FlipBuf::Count)];
            ConstantBufferView m_ConstantBufferView[static_cast<size_t>(FlipCBV::Count)];
            UploadBuffer m_ConstantUploadBuffer[static_cast<size_t>(FlipUploadBuf::Count)];

            FlippedResources() : m_BackBuffer(nullptr) {};
        };

        FlippedResources m_FlippedResources[BLK_IN_FLIGHT_FRAMES];
    };

    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::Tex);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::Buf);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::SRV);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::RTV);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::DSV);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::VBV);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::IBV);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::RootSig);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::DescHeap);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::FlipBuf);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::FlipCBV);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::FlipUploadBuf);
    BLK_DECLARE_ENUM_OPERATOR_PLUS(ResourceContainer::DefaultRootSigBindPoints);

}
