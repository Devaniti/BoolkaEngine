#pragma once
#include "CopyCommandList.h"

namespace Boolka
{

    class ComputeCommandList : public CopyCommandList
    {
    protected:
        ComputeCommandList() {};
        ~ComputeCommandList() {};
    };

}
