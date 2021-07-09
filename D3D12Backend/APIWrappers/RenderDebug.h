#pragma once

namespace Boolka

{

    class RenderDebug
    {
    public:
        RenderDebug() = default;
        ~RenderDebug() = default;

        bool Initialize();
        void Unload();

#ifdef BLK_RENDER_DEBUG
        static void SetDebugName(ID3D12DeviceChild* object, const wchar_t* format, ...);
#else
        static void SetDebugName(ID3D12DeviceChild* object, const wchar_t* format, ...){};
#endif

    private:
    };

} // namespace Boolka
