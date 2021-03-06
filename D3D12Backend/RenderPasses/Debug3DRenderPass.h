#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/PipelineState/GraphicPipelineState.h"
#include "APIWrappers/Resources/Buffers/UploadBuffer.h"
#include "APIWrappers/Resources/Buffers/Views/ConstantBufferView.h"
#include "APIWrappers/Resources/Buffers/Views/IndexBufferView.h"
#include "APIWrappers/Resources/Buffers/Views/VertexBufferView.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/DepthStencilView.h"
#include "APIWrappers/RootSignature.h"
#include "RenderPass.h"

namespace Boolka
{

    class Debug3DRenderPass : public RenderPass
    {
    public:
        Debug3DRenderPass();
        ~Debug3DRenderPass();

        bool Initialize(Device& device, RenderContext& renderContext) override;
        void Unload() override;

        bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) override;
        bool PrepareRendering() override;

    private:
        GraphicPipelineState m_PSO;
        UploadBuffer m_VertexBuffer;
        VertexBufferView m_VertexBufferView;
        UploadBuffer m_VertexInstanceBuffer;
        VertexBufferView m_VertexInstanceBufferView;
        UploadBuffer m_IndexBuffer;
        IndexBufferView m_IndexBufferView;

        float m_CurrentAngle;
    };

} // namespace Boolka
