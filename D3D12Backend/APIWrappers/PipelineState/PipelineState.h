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
        ID3D12PipelineState* Get() { BLK_ASSERT(m_PipelineState != nullptr); return m_PipelineState; };
        ID3D12PipelineState* operator->() { return Get(); };

        void Unload();

    private:
        ID3D12PipelineState* m_PipelineState;
    };
    
}
