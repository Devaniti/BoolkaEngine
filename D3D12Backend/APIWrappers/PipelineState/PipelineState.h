#pragma once

namespace Boolka
{

    class PipelineState
    {
    protected:
        PipelineState();
        ~PipelineState();

        bool Initialize(ID3D12PipelineState* pipelineState);

    public:
        ID3D12PipelineState* Get();
        ID3D12PipelineState* operator->();

        void Unload();

    private:
        ID3D12PipelineState* m_PipelineState;
    };

} // namespace Boolka
