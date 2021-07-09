#pragma once

#ifdef BLK_ENABLE_STATS

namespace Boolka
{

    class ImguiGraphHelper
    {
    public:
        ImguiGraphHelper();
        ~ImguiGraphHelper() = default;

        void PushValue(float value);
        void CalculateMinMaxAvg(float& minValue, float& maxValue, float& avgValue);
        void Render(float minValue, float maxValue, float width, float height);
        void PushValueAndRender(float value, const char* header, float width, float height);

        static const size_t ms_HistoryCount = 300;

    private:
        float m_HistoryBuffer[ms_HistoryCount];
    };

} // namespace Boolka

#endif
