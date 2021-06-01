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
            ShadowMapLight0,
            ShadowMapSun = ShadowMapLight0 + BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT,
            Count
        };

        BatchManager();
        ~BatchManager() = default;

        bool Initialize(const Scene& scene);
        void Unload();

        bool PrepareBatches(const RenderFrameContext& frameContext, const Scene& scene);

        bool Render(CommandList& commandList, BatchType batch) const;

    private:
        struct DrawData
        {
            UINT objectOffset;
            UINT objectCount;
        };

        // used to sort objects by distance
        struct SortingData
        {
            UINT objectIndex;
            float distance;
        };

        DrawData m_batches[static_cast<size_t>(BatchType::Count)];
    };

    BLK_DECLARE_ENUM_OPERATORS(BatchManager::BatchType);

} // namespace Boolka
