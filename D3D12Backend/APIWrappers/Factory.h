#pragma once

namespace Boolka
{

    class [[nodiscard]] Factory
    {
    public:
        Factory();
        ~Factory();

        [[nodiscard]] IDXGIFactory7* Get();
        [[nodiscard]] IDXGIFactory7* operator->();

        bool Initialize();
        void Unload();

    private:
        IDXGIFactory7* m_Factory;
    };

} // namespace Boolka
