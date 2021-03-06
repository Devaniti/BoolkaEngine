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

        BatchManager() = default;
        ~BatchManager();

        bool Initialize(const Scene& scene);
        void Unload();

        bool PrepareBatches(const RenderFrameContext& frameContext, const Scene& scene);
        bool Render(CommandList& commandList, BatchType batch);

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
        void GetBatchRange(BatchType batch, const Scene& scene, UINT& startIndex, UINT& endIndex);
        void GetBatchView(BatchType batch, const RenderFrameContext& frameContext,
                          Vector3& cameraCoord, Matrix4x4& viewProjMatrix);
        void CullObjects(const Scene& scene, Matrix4x4 viewProjMatrix, Vector3 cameraCoord,
                         UINT startIndex, UINT endIndex, std::vector<SortingData>& culledObjects);
        void SortObjects(BatchType batch, std::vector<SortingData> culledObjects);

        std::vector<DrawData>& GetBatch(BatchType id)
        {
            return m_batches[static_cast<size_t>(id)];
        };

        std::vector<DrawData> m_batches[static_cast<size_t>(BatchType::Count)];
        void GenerateDrawData(BatchType batch, const Scene& scene,
                              const std::vector<SortingData>& culledObjects);
    };

    BLK_DECLARE_ENUM_OPERATORS(BatchManager::BatchType);

} // namespace Boolka
