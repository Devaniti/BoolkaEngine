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

    bool PipelineState::Initialize(ID3D12PipelineState* pipelineState)
    {
        BLK_ASSERT(m_PipelineState == nullptr);

        m_PipelineState = pipelineState;
        return true;
    }

    void PipelineState::Unload()
    {
        BLK_ASSERT(m_PipelineState != nullptr);

        m_PipelineState->Release();
        m_PipelineState = nullptr;
    }

}