#pragma once

namespace Boolka
{

    class DStorageFactory;

    class [[nodiscard]] DStorageFile
    {
    public:
        DStorageFile();
        ~DStorageFile();

        [[nodiscard]] IDStorageFile* Get();
        [[nodiscard]] IDStorageFile* operator->();

        bool OpenFile(DStorageFactory& factory, const wchar_t* filename);
        void CloseFile();

    private:
        IDStorageFile* m_File;
    };

} // namespace Boolka
