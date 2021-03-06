#pragma once
#include "ComputeCommandList.h"

namespace Boolka
{

    class GraphicCommandList : public ComputeCommandList
    {
    protected:
        GraphicCommandList() = default;
        ~GraphicCommandList() = default;
    };

} // namespace Boolka
