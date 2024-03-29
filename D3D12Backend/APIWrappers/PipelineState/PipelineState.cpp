#include "stdafx.h"

#include "PipelineState.h"

namespace Boolka
{

    PipelineState::PipelineState()
        : m_PipelineState(nullptr)
    {
    }

    PipelineState::~PipelineState()
    {
        BLK_ASSERT(m_PipelineState == nullptr);
    }

    void PipelineState::Initialize(ID3D12PipelineState* pipelineState)
    {
        BLK_ASSERT(m_PipelineState == nullptr);
        m_PipelineState = pipelineState;
    }

    ID3D12PipelineState* PipelineState::Get()
    {
        BLK_ASSERT(m_PipelineState != nullptr);
        return m_PipelineState;
    }

    ID3D12PipelineState* PipelineState::operator->()
    {
        return Get();
    }

    void PipelineState::Unload()
    {
        BLK_ASSERT(m_PipelineState != nullptr);

        m_PipelineState->Release();
        m_PipelineState = nullptr;
    }

} // namespace Boolka