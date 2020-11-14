#pragma once
#include "APIWrappers/Resources/Buffers/VertexBuffer.h"
#include "APIWrappers/Resources/Buffers/IndexBuffer.h"
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/ShaderResourceView.h"

namespace Boolka
{

    class SceneData;
    class RenderEngineContext;

    class Scene
    {
    public:
        Scene();
        virtual ~Scene();

        bool Initialize(Device& device, SceneData& sceneData, RenderEngineContext& engineContext);
        void Unload();

        UINT GetVertexBufferSize() { return m_VertexBufferSize; };
        VertexBuffer& GetVertexBuffer() { return m_VertexBuffer; };
        UINT GetIndexBufferSize() { return m_IndexBufferSize; };
        IndexBuffer& GetIndexBuffer() { return m_IndexBuffer; };
        DescriptorHeap& GetSRVDescriptorHeap() { return m_SRVDescriptorHeap; };

    private:
        UINT m_VertexBufferSize;
        UINT m_IndexBufferSize;
        VertexBuffer m_VertexBuffer;
        IndexBuffer m_IndexBuffer;
        DescriptorHeap m_SRVDescriptorHeap;
        std::vector<Texture2D> m_Textures;
        std::vector<ShaderResourceView> m_SRVs;
    };

}
