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
            ShadowMapSun,
            ShadowMapLight0,
            Count = ShadowMapLight0 + BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT
        };

        // TODO design good place to put it
        enum class ViewType
        {
            MainView,
            ShadowMapSun,
            ShadowMapLight0,
            Count = ShadowMapLight0 + BLK_MAX_LIGHT_COUNT * BLK_TEXCUBE_FACE_COUNT,
        };

        BatchManager() = default;
        ~BatchManager() = default;

        bool Initialize(Device& device, const Scene& scene, RenderEngineContext& engineContext);
        void Unload();

        bool PrepareBatches(const RenderFrameContext& frameContext, const Scene& scene);

        bool Render(CommandList& commandList, RenderContext& renderContext, BatchType batch);

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

        DrawData m_Batches[static_cast<size_t>(BatchType::Count)];
        CommandSignature m_CommandSignature;
    };

    BLK_DECLARE_ENUM_OPERATORS(BatchManager::BatchType);
    BLK_DECLARE_ENUM_OPERATORS(BatchManager::ViewType);

} // namespace Boolka
