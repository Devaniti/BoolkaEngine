#pragma once

namespace Boolka
{

    class [[nodiscard]] RenderBackend
    {
    public:
        virtual ~RenderBackend() = default;

        virtual bool Initialize(const wchar_t* folderPath) = 0;
        virtual void Unload() = 0;
        virtual bool Present() = 0;
        virtual bool RenderFrame() = 0;

        [[nodiscard]] static RenderBackend* CreateRenderBackend();
        static void DeleteRenderBackend(RenderBackend* object);
        virtual void UnloadScene() = 0;
    };

} // namespace Boolka
