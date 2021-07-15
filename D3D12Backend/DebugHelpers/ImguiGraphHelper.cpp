#include "stdafx.h"

#include "ImguiGraphHelper.h"

#ifdef BLK_ENABLE_STATS

#include "ThirdParty/imgui/imgui.h"

namespace Boolka
{

    ImguiGraphHelper::ImguiGraphHelper()
        : m_HistoryBuffer{}
    {
    }

    void ImguiGraphHelper::PushValue(float value)
    {
        std::shift_left(std::begin(m_HistoryBuffer), std::end(m_HistoryBuffer), 1);
        m_HistoryBuffer[ms_HistoryCount - 1] = value;
    }

    void ImguiGraphHelper::CalculateMinMaxAvg(float& minValue, float& maxValue, float& avgValue)
    {
        float currentMin = FLT_MAX;
        float currentMax = FLT_MIN;
        float sum = 0.0f;

        for (float value : m_HistoryBuffer)
        {
            currentMin = std::min(value, currentMin);
            currentMax = std::max(value, currentMax);
            sum += value;
        }

        minValue = currentMin;
        maxValue = currentMax;
        avgValue = sum / ms_HistoryCount;
    }

    void ImguiGraphHelper::Render(float minValue, float maxValue, float width, float height)
    {
        // Currently scaling from 0 to maxValue, so minValue is unused
        BLK_UNUSED_VARIABLE(minValue);
        // Scale at least from 0 to 0.01, to avoid flickering when we have all values close to 0
        maxValue = std::max(0.01f, maxValue);

        ImGui::PlotLines("", m_HistoryBuffer, ms_HistoryCount, 0, nullptr, 0.0f, maxValue,
                         ImVec2(width, height));
    }

    void ImguiGraphHelper::PushValueAndRender(float value, const char* header, float width, float height)
    {
        PushValue(value);
        float minValue, maxValue, avgValue;
        CalculateMinMaxAvg(minValue, maxValue, avgValue);
        if (header != nullptr)
        {
            ImGui::Text("%s min %.1f, max %.1f, avg %.1f", header, minValue, maxValue, avgValue);
        }
        Render(minValue, maxValue, width, height);
    }

} // namespace Boolka

#endif
