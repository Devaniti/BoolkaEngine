#include "stdafx.h"

#include "SceneDataReader.h"

namespace Boolka
{

    SceneDataReader::SceneDataReader()
        : m_Header{}
    {
    }

    SceneDataReader::~SceneDataReader()
    {
        BLK_ASSERT(m_Header.m_Size == 0);
        BLK_ASSERT(m_Header.m_Data == nullptr);
    }

    bool SceneDataReader::OpenScene(Device& device, const wchar_t* folderPath)
    {
        BLK_ASSERT(m_Header.m_Size == 0);
        BLK_ASSERT(m_Header.m_Data == nullptr);

        std::wstring headerFile;
        CombinePath(folderPath, BLK_SCENE_HEADER_FILENAME, headerFile);

        m_Header = DebugFileReader::ReadFile(headerFile.c_str());
        BLK_CRITICAL_ASSERT(m_Header.m_Data);

        unsigned char* data = static_cast<unsigned char*>(m_Header.m_Data);

        const HeaderStart* headerStart = ptr_static_cast<const HeaderStart*>(data);
        BLK_CRITICAL_ASSERT(headerStart->formatHeader.IsValid());

        const SceneData::SceneHeader& sceneHeader = headerStart->sceneHeader;

        BLK_CRITICAL_ASSERT(sceneHeader.vertex1Size != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.vertex2Size != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.vertexIndirectionSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.indexSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.meshletsSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.meshletsCullSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.objectsSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.materialsSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.rtIndiciesSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.rtObjectIndexOffsetSize != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.objectCount != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.opaqueCount != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.skyBoxResolution != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.skyBoxMipCount != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.textureCount != 0);
        BLK_CRITICAL_ASSERT(sceneHeader.objectCount < Scene::MaxObjectCount);

        DStorageQueue& queue = device.GetDStorageQueue();
        std::wstring dataFile;
        CombinePath(folderPath, BLK_SCENE_DATA_FILENAME, dataFile);
        bool res = m_SceneDataFile.OpenFile(device.GetDStorageFactory(), dataFile.c_str());
        BLK_ASSERT(res);

        return true;
    }

    void SceneDataReader::CloseReader()
    {
        BLK_ASSERT(m_Header.m_Size != 0);
        BLK_ASSERT(m_Header.m_Data != nullptr);
        DebugFileReader::FreeMemory(m_Header);
        m_Header = {};
        m_SceneDataFile.CloseFile();
    }

    SceneDataReader::HeaderWrapper SceneDataReader::GetHeaderWrapper()
    {
        unsigned char* data = static_cast<unsigned char*>(m_Header.m_Data);
        const HeaderStart* headerStart = ptr_static_cast<const HeaderStart*>(data);
        const SceneData::SceneHeader* sceneHeader = &headerStart->sceneHeader;

        data += sizeof(HeaderStart);

        const SceneData::TextureHeader* textureHeader =
            ptr_static_cast<const SceneData::TextureHeader*>(data);

        data += sizeof(SceneData::TextureHeader) * sceneHeader->textureCount;

        const SceneData::CPUObjectHeader* cpuObjectHeader =
            ptr_static_cast<const SceneData::CPUObjectHeader*>(data);

        return HeaderWrapper{sceneHeader, textureHeader, cpuObjectHeader};
    }

    DStorageFile& SceneDataReader::GetSceneDataFile()
    {
        return m_SceneDataFile;
    }

} // namespace Boolka
