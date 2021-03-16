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
        ~BatchManager();

        bool Initialize(const Scene& scene);
        void Unload();

        bool PrepareBatches(const RenderFrameContext& frameContext, const Scene& scene);
        bool NeedRender(BatchType batch) const;
        bool Render(CommandList& commandList, BatchType batch) const;

        size_t GetCount(BatchType batch);

    private:
        struct DrawData
        {
            UINT indexCount;
            UINT startIndex;
        };

        // used to sort objects by distance
        struct SortingData
        {
            UINT objectIndex;
            float distance;
        };

        bool IsBatchEnabled(BatchType batch, const RenderFrameContext& frameContext);
        void CalculateRequiredBatches(const RenderFrameContext& frameContext);
        void GetBatchRange(BatchType batch, const Scene& scene, UINT& startIndex, UINT& endIndex);
        void GetBatchView(BatchType batch, const RenderFrameContext& frameContext,
                          Vector4& cameraCoord, Matrix4x4& viewProjMatrix);
        void CullObjects(const Scene& scene, Matrix4x4 viewProjMatrix, Vector4 cameraCoord,
                         UINT startIndex, UINT endIndex, std::vector<SortingData>& culledObjects);
        void SortObjects(BatchType batch, std::vector<SortingData> culledObjects);
        std::vector<DrawData>& GetBatch(BatchType id);
        const std::vector<DrawData>& GetBatch(BatchType id) const;
        void GenerateDrawData(BatchType batch, const Scene& scene,
                              const std::vector<SortingData>& culledObjects);
        void ClearBatch(BatchType batch);

        std::vector<DrawData> m_batches[static_cast<size_t>(BatchType::Count)];
        bool m_requiredBatches[static_cast<size_t>(BatchType::Count)];
    };

    BLK_DECLARE_ENUM_OPERATORS(BatchManager::BatchType);

} // namespace Boolka
