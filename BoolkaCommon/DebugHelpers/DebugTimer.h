#pragma once

namespace Boolka
{

    class [[nodiscard]] DebugTimer
    {
    public:
        DebugTimer();
        ~DebugTimer();

        bool Start();
        // return time in seconds
        float Stop();

    private:
        LARGE_INTEGER m_LastTimestamp;
        LARGE_INTEGER m_Frequency;
    };

} // namespace Boolka
