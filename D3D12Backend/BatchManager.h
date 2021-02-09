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
            Count
        };

        BatchManager() = default;
        ~BatchManager();

        bool Initialize(const Scene& scene);
        void Unload();

        bool PrepareBatches(const RenderFrameContext& frameContext, Scene& scene);
        bool Render(CommandList& commandList, BatchType batch);
    private:

        struct DrawData
        {
            UINT indexCount;
            UINT startIndex;
        };

        std::vector<DrawData> m_batches[static_cast<size_t>(BatchType::Count)];
    };

}
