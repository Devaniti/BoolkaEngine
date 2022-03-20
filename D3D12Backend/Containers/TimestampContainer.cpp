#include "stdafx.h"

#include "TimestampContainer.h"

#include "APIWrappers/Device.h"

namespace Boolka
{

    TimestampContainer::TimestampContainer()
        : m_TimestampFrequency(0)
    {
    }

    TimestampContainer::~TimestampContainer()
    {
        BLK_ASSERT(m_TimestampFrequency == 0);
    }

    bool TimestampContainer::Initialize(Device& device,
                                        GraphicCommandListImpl& initializationCommandList)
    {
        BLK_ASSERT(m_TimestampFrequency == 0);

        m_QueryHeap.Initialize(device, D3D12_QUERY_HEAP_TYPE_TIMESTAMP,
                               static_cast<UINT>(Markers::Count));
        BLK_INITIALIZE_ARRAY(m_ReadbackBuffer, device,
                             sizeof(UINT64) * static_cast<UINT>(Markers::Count));
        device.GetGraphicQueue()->GetTimestampFrequency(&m_TimestampFrequency);

        for (UINT i = 0; i < static_cast<UINT>(Markers::Count); ++i)
        {
            initializationCommandList->EndQuery(m_QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, i);
        }

        return true;
    }

    void TimestampContainer::Unload()
    {
        BLK_ASSERT(m_TimestampFrequency != 0);

        m_QueryHeap.Unload();
        BLK_UNLOAD_ARRAY(m_ReadbackBuffer);
        m_TimestampFrequency = 0;
    }

    void TimestampContainer::Mark(GraphicCommandListImpl& commandList, Markers marker)
    {
        commandList->EndQuery(m_QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP,
                              static_cast<UINT>(marker));
    }

    void TimestampContainer::FinishFrame(GraphicCommandListImpl& commandList,
                                         RenderContext& renderContext, UINT frameIndex)
    {
        BLK_ASSERT(m_TimestampFrequency != 0);

        auto [engineContext, frameContext, threadContext] = renderContext.GetContexts();

        FrameStats& frameStats = frameContext.GetFrameStats();
        ReadbackBuffer& readbackBuffer = m_ReadbackBuffer[frameIndex];

        FrameStats::GPUTimes gpuTimes{};

        UINT64 markers[static_cast<size_t>(Markers::Count)];
        readbackBuffer.Readback(markers, sizeof(markers));

        for (size_t i = 0; i < static_cast<size_t>(Markers::Count) - 1; ++i)
        {
            // divide in double precision to get more accurate results
            gpuTimes.Markers[i] = static_cast<float>((markers[i + 1] - markers[i]) /
                                                     static_cast<double>(m_TimestampFrequency));
        }

        gpuTimes.Markers[static_cast<size_t>(Markers::Count) - 1] =
            static_cast<float>((markers[static_cast<size_t>(Markers::EndFrame)] -
                                markers[static_cast<size_t>(Markers::BeginFrame)]) /
                               static_cast<double>(m_TimestampFrequency));

        frameStats.gpuTimes = gpuTimes;
        frameStats.gpuTimesStable = frameStats.gpuTimesStable * 0.95f + gpuTimes * 0.05f;

        commandList->ResolveQueryData(m_QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0,
                                      static_cast<UINT>(Markers::Count), readbackBuffer.Get(), 0);
    }

} // namespace Boolka
