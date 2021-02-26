#pragma once

namespace Boolka
{

    class CommandList;
    class Scene;
    class RenderFrameContext;

    class BatchManager
    {
    public:
        enum class BatchType
        {
            Opaque,
            Transparent,
            ShadowMap0,
            Count = ShadowMap0 + BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT
        };

        BatchManager() = default;
        ~BatchManager();

        bool Initialize(const Scene& scene);
        void Unload();

        bool PrepareBatches(RenderFrameContext& frameContext, Scene& scene);
        bool Render(CommandList& commandList, BatchType batch);
    private:
        struct DrawData
        {
            UINT indexCount;
            UINT startIndex;
        };
        std::vector<DrawData>& GetBatch(BatchType id) { return m_batches[static_cast<size_t>(id)]; };

        std::vector<DrawData> m_batches[static_cast<size_t>(BatchType::Count)];
    };

    BLK_DECLARE_ENUM_OPERATOR_PLUS(BatchManager::BatchType);
    BLK_DECLARE_ENUM_OPERATOR_MINUS(BatchManager::BatchType);

}
