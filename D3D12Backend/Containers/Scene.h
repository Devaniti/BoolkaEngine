#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/ShaderResourceView.h"
#include "APIWrappers/ResourceHeap.h"
#include "APIWrappers/Resources/Buffers/CommandSignature.h"
#include "APIWrappers/Resources/Buffers/Views/VertexBufferView.h"
#include "APIWrappers/Resources/Buffers/Views/IndexBufferView.h"

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
        Buffer& GetVertexBuffer() { return m_VertexBuffer; };
        VertexBufferView& GetVertexBufferView() { return m_VertexBufferView; };
        UINT GetIndexBufferSize() { return m_IndexBufferSize; };
        Buffer& GetIndexBuffer() { return m_IndexBuffer; };
        IndexBufferView& GetIndexBufferView() { return m_IndexBufferView; };
        UINT GetCullingBufferSize() { return m_CullingBufferSize; };
        Buffer& GetCullingBuffer() { return m_CullingBuffer; };
        UINT GetIndexCount() { return m_IndexCount; };
        UINT GetObjectCount() { return m_ObjectCount; };
        DescriptorHeap& GetSRVDescriptorHeap() { return m_SRVDescriptorHeap; };

    private:
        UINT m_VertexBufferSize;
        UINT m_IndexBufferSize;
        UINT m_CullingBufferSize;
        UINT m_IndexCount;
        UINT m_ObjectCount;
        Buffer m_VertexBuffer;
        Buffer m_IndexBuffer;
        Buffer m_CullingBuffer;
        Buffer m_DrawIndirectBuffer;
        VertexBufferView m_VertexBufferView;
        IndexBufferView m_IndexBufferView;
        CommandSignature m_CommandSignature;
        DescriptorHeap m_SRVDescriptorHeap;
        ResourceHeap m_ResourceHeap;
        std::vector<Texture2D> m_Textures;
        std::vector<ShaderResourceView> m_SRVs;
    };

}
