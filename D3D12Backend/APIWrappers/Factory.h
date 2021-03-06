#pragma once

namespace Boolka
{

    class Factory
    {
    public:
        Factory();
        ~Factory();

        IDXGIFactory7* Get();
        IDXGIFactory7* operator->();

        bool Initialize();
        void Unload();

    private:
        IDXGIFactory7* m_Factory;
    };

} // namespace Boolka
