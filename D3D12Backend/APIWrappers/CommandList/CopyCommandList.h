#pragma once
#include "CommandList.h"

namespace Boolka
{

    class [[nodiscard]] CopyCommandList : public CommandList
    {
    protected:
        CopyCommandList() = default;
        ~CopyCommandList() = default;
    };

} // namespace Boolka
