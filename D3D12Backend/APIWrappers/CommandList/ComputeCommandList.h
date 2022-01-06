#pragma once
#include "CopyCommandList.h"

namespace Boolka
{

    class [[nodiscard]] ComputeCommandList : public CopyCommandList
    {
    protected:
        ComputeCommandList() = default;
        ~ComputeCommandList() = default;
    };

} // namespace Boolka
