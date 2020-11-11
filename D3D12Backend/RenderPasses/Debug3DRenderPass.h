#pragma once
#include "RenderPass.h"

#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "APIWrappers/RootSignature.h"
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Resources/Buffers/VertexBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/VertexBufferView.h"
#include "APIWrappers/Resources/Buffers/IndexBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/IndexBufferView.h"
#include "APIWrappers/Resources/Buffers/ConstantBuffer.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/ConstantBufferView.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"

namespace Boolka
{

    class Debug3DRenderPass :
        public RenderPass
    {
    public:
        Debug3DRenderPass();
        ~Debug3DRenderPass();

        bool Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;
        VertexBuffer m_VertexBuffer;
        VertexBufferView m_VertexBufferView;
        VertexBuffer m_VertexInstanceBuffer;
        VertexBufferView m_VertexInstanceBufferView;
        IndexBuffer m_IndexBuffer;
        IndexBufferView m_IndexBufferView;
        ConstantBuffer m_ConstantBuffers[BLK_IN_FLIGHT_FRAMES];
        UploadBuffer m_UploadBuffers[BLK_IN_FLIGHT_FRAMES];
        ConstantBufferView m_ConstantBufferViews[BLK_IN_FLIGHT_FRAMES];
        Texture2D m_DepthBuffer;
        DepthStencilView m_DSV;

        float m_CurrentAngle;
    };

}

