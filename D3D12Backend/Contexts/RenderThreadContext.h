#pragma once

#include "APIWrappers/CommandAllocator/GraphicCommandAllocator.h"

namespace Boolka
{

    class Device;

    class [[nodiscard]] RenderThreadContext
    {
    public:
        RenderThreadContext();
        ~RenderThreadContext();

        bool Initialize(Device& device);
        void Unload();

        [[nodiscard]] GraphicCommandListImpl& GetGraphicCommandList();

        void FlipFrame(UINT frameIndex);

    private:
        GraphicCommandAllocator* m_CurrentGraphicCommandAllocator;
        GraphicCommandListImpl* m_CurrentGraphicCommandList;
        GraphicCommandAllocator m_GraphicCommandAllocator[BLK_IN_FLIGHT_FRAMES];
        GraphicCommandListImpl m_GraphicCommandList[BLK_IN_FLIGHT_FRAMES];
    };

} // namespace Boolka
