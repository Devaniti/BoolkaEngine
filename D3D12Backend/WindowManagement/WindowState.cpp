#include "stdafx.h"

#include "WindowState.h"

namespace Boolka
{

    WindowState WindowState::GetDefault()
    {
        WindowState result{};
        result.x = result.y = 0;
        result.width = ::GetSystemMetrics(SM_CXSCREEN);
        result.height = ::GetSystemMetrics(SM_CYSCREEN);
        result.presentInterval = 0;
        result.windowMode = WindowMode::Borderless;
        return result;
    }

} // namespace Boolka