#pragma once

namespace Boolka
{

    class [[nodiscard]] PipelineState
    {
    protected:
        PipelineState();
        ~PipelineState();

        void Initialize(ID3D12PipelineState* pipelineState);

    public:
        [[nodiscard]] ID3D12PipelineState* Get();
        [[nodiscard]] ID3D12PipelineState* operator->();

        void Unload();

    private:
        ID3D12PipelineState* m_PipelineState;
    };

} // namespace Boolka
