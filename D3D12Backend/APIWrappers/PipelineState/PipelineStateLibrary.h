#pragma once

#ifdef BLK_ENABLE_PIPELINE_LIBRARY

namespace Boolka
{

    struct MemoryBlock;
    struct RenderCacheContainer;
    class PipelineState;

    class [[nodiscard]] PipelineStateLibrary
    {
    public:
        PipelineStateLibrary();
        ~PipelineStateLibrary();

        bool Initialize(Device& device, RenderCacheContainer& renderCache);
        bool Initialize(Device& device, const MemoryBlock& CachedPSOLibrary);
        bool Initialize(Device& device);

        [[nodiscard]] ID3D12PipelineLibrary1* Get();
        [[nodiscard]] ID3D12PipelineLibrary1* operator->();

        void Store(Device& device, const wchar_t* name, PipelineState& PSO);
        void SaveToDisk(const wchar_t* filename);

        void Unload();

    private:
        // Clears saved PSOs, we need to clear whole library if any PSO changed
        bool Reset(Device& device);

        bool Serialize(MemoryBlock& data);
        void FreeData(MemoryBlock& data);

        ID3D12PipelineLibrary1* m_PipelineStateLibrary;
    };

} // namespace Boolka

#endif
