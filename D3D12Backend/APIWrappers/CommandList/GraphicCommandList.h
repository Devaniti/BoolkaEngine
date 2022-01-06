#pragma once
#include "ComputeCommandList.h"

namespace Boolka
{

    class [[nodiscard]] GraphicCommandList : public ComputeCommandList
    {
    protected:
        GraphicCommandList() = default;
        ~GraphicCommandList() = default;
    };

} // namespace Boolka
