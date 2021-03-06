#pragma once
#include "APIWrappers/DescriptorHeap.h"
#include "APIWrappers/ResourceHeap.h"
#include "APIWrappers/Resources/Buffers/CommandSignature.h"
#include "APIWrappers/Resources/Buffers/Views/IndexBufferView.h"
#include "APIWrappers/Resources/Buffers/Views/VertexBufferView.h"
#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "APIWrappers/Resources/Textures/Views/ShaderResourceView.h"
#include "BatchManager.h"
#include "Containers/Streaming/SceneData.h"

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

        UINT GetVertexBufferSize() const;
        Buffer& GetVertexBuffer();
        VertexBufferView& GetVertexBufferView();
        UINT GetIndexBufferSize() const;
        Buffer& GetIndexBuffer();
        IndexBufferView& GetIndexBufferView();
        UINT GetIndexCount() const;
        // All opaque objects placed before all transparent objects
        // So objects in range [0, m_OpaqueObjectCount) - are opaque
        // And objects in range [m_OpaqueObjectCount, m_ObjectCount) - are
        // transparent
        UINT GetObjectCount() const;
        UINT GetOpaqueObjectCount() const;
        DescriptorHeap& GetSRVDescriptorHeap();
        BatchManager& GetBatchManager();
        std::vector<SceneData::ObjectHeader>& GetObjects();
        const std::vector<SceneData::ObjectHeader>& GetObjects() const;

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
        BatchManager m_BatchManager;
        std::vector<Texture2D> m_Textures;
        std::vector<ShaderResourceView> m_SRVs;
        std::vector<SceneData::ObjectHeader> m_Objects;
    };

} // namespace Boolka
