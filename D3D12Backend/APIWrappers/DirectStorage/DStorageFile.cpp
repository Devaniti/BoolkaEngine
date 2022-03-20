#include "stdafx.h"

#include "DStorageFile.h"

#include "DStorageFactory.h"

namespace Boolka
{

    DStorageFile::DStorageFile()
        : m_File(nullptr)
    {
    }

    DStorageFile::~DStorageFile()
    {
        BLK_ASSERT(m_File == nullptr);
    }

    IDStorageFile* DStorageFile::Get()
    {
        BLK_ASSERT(m_File != nullptr);
        return m_File;
    }

    IDStorageFile* DStorageFile::operator->()
    {
        return Get();
    }

    bool DStorageFile::OpenFile(DStorageFactory& factory, const wchar_t* filename)
    {
        BLK_ASSERT(m_File == nullptr);
        HRESULT hr = factory->OpenFile(filename, IID_PPV_ARGS(&m_File));
        return SUCCEEDED(hr);
    }

    void DStorageFile::CloseFile()
    {
        BLK_ASSERT(m_File != nullptr);
        m_File->Release();
        m_File = nullptr;
    }

} // namespace Boolka
