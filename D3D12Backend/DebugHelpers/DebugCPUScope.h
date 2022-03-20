#pragma once

namespace Boolka
{

    class [[nodiscard]] DebugCPUScope
    {
    public:
        DebugCPUScope(const char* name);
        ~DebugCPUScope();

    private:
        void StartEvent(const char* name);
        void EndEvent();
    };

} // namespace Boolka
