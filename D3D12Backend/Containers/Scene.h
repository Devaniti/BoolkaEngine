#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/ShaderResourceView.h"
#include "APIWrappers/ResourceHeap.h"
#include "APIWrappers/Resources/Buffers/CommandSignature.h"
#include "APIWrappers/Resources/Buffers/Views/VertexBufferView.h"
#include "APIWrappers/Resources/Buffers/Views/IndexBufferView.h"
#include "Containers/Streaming/SceneData.h"
#include "CullingManager.h"
#include "BatchManager.h"

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

        UINT GetVertexBufferSize() const { return m_VertexBufferSize; };
        Buffer& GetVertexBuffer() { return m_VertexBuffer; };
        VertexBufferView& GetVertexBufferView() { return m_VertexBufferView; };
        UINT GetIndexBufferSize() const { return m_IndexBufferSize; };
        Buffer& GetIndexBuffer() { return m_IndexBuffer; };
        IndexBufferView& GetIndexBufferView() { return m_IndexBufferView; };
        UINT GetIndexCount() const { return m_IndexCount; };
        // All opaque objects placed before all transparent objects
        // So objects in range [0, m_OpaqueObjectCount) - are opaque
        // And objects in range [m_OpaqueObjectCount, m_ObjectCount) - are transparent
        UINT GetObjectCount() const { return m_ObjectCount; };
        UINT GetOpaqueObjectCount() const { return m_OpaqueObjectCount; };
        DescriptorHeap& GetSRVDescriptorHeap() { return m_SRVDescriptorHeap; };
        CullingManager& GetCullingManager() { return m_CullingManager; };
        BatchManager& GetBatchManager() { return m_BatchManager; };
        std::vector<SceneData::ObjectHeader>& GetObjects() { return m_Objects; };

    private:
        UINT m_VertexBufferSize;
        UINT m_IndexBufferSize;
        UINT m_IndexCount;
        UINT m_ObjectCount;
        UINT m_OpaqueObjectCount;
        Buffer m_VertexBuffer;
        Buffer m_IndexBuffer;
        Buffer m_DrawIndirectBuffer;
        VertexBufferView m_VertexBufferView;
        IndexBufferView m_IndexBufferView;
        CommandSignature m_CommandSignature;
        DescriptorHeap m_SRVDescriptorHeap;
        ResourceHeap m_ResourceHeap;
        CullingManager m_CullingManager;
        BatchManager m_BatchManager;
        std::vector<Texture2D> m_Textures;
        std::vector<ShaderResourceView> m_SRVs;
        std::vector<SceneData::ObjectHeader> m_Objects;
    };

}
