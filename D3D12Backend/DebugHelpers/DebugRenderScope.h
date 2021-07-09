#pragma once

#ifdef BLK_RENDER_DEBUG

namespace Boolka
{

    class DebugRenderScope
    {
    public:
        DebugRenderScope(CommandList& commandList, const char* name);
        ~DebugRenderScope();

    private:
        void StartEvent(const char* name);
        void EndEvent();

        ID3D12GraphicsCommandList* m_commandList;
    };

} // namespace Boolka

#endif
