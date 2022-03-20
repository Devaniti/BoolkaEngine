#pragma once

#ifdef BLK_USE_PIX_MARKERS

namespace Boolka
{

    class [[nodiscard]] DebugRenderScope
    {
    public:
        DebugRenderScope(CommandList& commandList, const char* name);
        ~DebugRenderScope();

    private:
        void StartEvent(const char* name);
        void EndEvent();

        ID3D12GraphicsCommandList* m_CommandList;
    };

} // namespace Boolka

#endif
