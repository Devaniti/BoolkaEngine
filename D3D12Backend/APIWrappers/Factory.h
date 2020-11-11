#pragma once

namespace Boolka
{

    class Factory
    {
    public:
        Factory();
        ~Factory();

        IDXGIFactory7* Get() { BLK_ASSERT(m_Factory != nullptr); return m_Factory; };
        IDXGIFactory7* operator->() { return Get(); };

        bool Initialize();
        void Unload();
    private:
        IDXGIFactory7* m_Factory;
    };

}
