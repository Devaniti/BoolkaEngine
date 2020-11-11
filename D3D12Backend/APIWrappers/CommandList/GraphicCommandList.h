#pragma once
#include "ComputeCommandList.h"

namespace Boolka
{

    class GraphicCommandList : public ComputeCommandList
    {
    protected:
        GraphicCommandList() {};
        ~GraphicCommandList() {};
    };

}
