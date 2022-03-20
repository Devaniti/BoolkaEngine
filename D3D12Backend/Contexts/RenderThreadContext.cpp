#include "stdafx.h"

#include "RenderThreadContext.h"

namespace Boolka
{

    RenderThreadContext::RenderThreadContext()
        : m_CurrentGraphicCommandAllocator(nullptr)
        , m_CurrentGraphicCommandList(nullptr)
    {
    }

    RenderThreadContext::~RenderThreadContext()
    {
        BLK_ASSERT(m_CurrentGraphicCommandList == nullptr);
        BLK_ASSERT(m_CurrentGraphicCommandAllocator == nullptr);
    }

    bool RenderThreadContext::Initialize(Device& device)
    {
        BLK_ASSERT(m_CurrentGraphicCommandList == nullptr);
        BLK_ASSERT(m_CurrentGraphicCommandAllocator == nullptr);

        BLK_CPU_SCOPE("RenderThreadContext::Initialize");

        for (size_t i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            bool res = m_GraphicCommandAllocator[i].Initialize(device);
            BLK_CRITICAL_ASSERT(res);
            res = m_GraphicCommandAllocator[i].InitializeCommandList(m_GraphicCommandList[i],
                                                                     device, nullptr);
            m_GraphicCommandList[i]->Close();
            BLK_CRITICAL_ASSERT(res);
        }

        return true;
    }

    void RenderThreadContext::Unload()
    {
        // Current command list/allocator may actually be null here, so no
        // assertion
        m_CurrentGraphicCommandList = nullptr;
        m_CurrentGraphicCommandAllocator = nullptr;

        for (size_t i = 0; i < BLK_IN_FLIGHT_FRAMES; ++i)
        {
            m_GraphicCommandList[i].Unload();
            m_GraphicCommandAllocator[i].Unload();
        }
    }

    GraphicCommandListImpl& RenderThreadContext::GetGraphicCommandList()
    {
        return *m_CurrentGraphicCommandList;
    }

    void RenderThreadContext::FlipFrame(UINT frameIndex)
    {
        m_CurrentGraphicCommandAllocator = &m_GraphicCommandAllocator[frameIndex];
        m_CurrentGraphicCommandList = &m_GraphicCommandList[frameIndex];

        m_CurrentGraphicCommandAllocator->Reset();
        m_CurrentGraphicCommandAllocator->ResetCommandList(*m_CurrentGraphicCommandList, nullptr);
    }

} // namespace Boolka
