#pragma once

namespace Boolka
{

    class [[nodiscard]] DStorageFactory
    {
    public:
        DStorageFactory();
        ~DStorageFactory();

        [[nodiscard]] IDStorageFactory* Get();
        [[nodiscard]] IDStorageFactory* operator->();

        bool Initialize();
        void Unload();

    private:
        IDStorageFactory* m_Factory;
    };

} // namespace Boolka
