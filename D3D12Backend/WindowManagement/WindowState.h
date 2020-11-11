#pragma once

namespace Boolka
{

    struct WindowState
    {
        enum class WindowMode
        {
            Windowed,
            Borderless,
            Fullscreen
        };

        int x;
        int y;
        int width;
        int height;
        int presentInterval; // aka VSYNC
        WindowMode windowMode;

        static WindowState GetDefault();
    };

    IS_PLAIN_DATA_ASSERT(WindowState);

}
