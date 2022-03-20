#pragma once
#include "APIWrappers/DirectStorage/DStorageFile.h"
#include "BoolkaCommon/Structures/AABB.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"
#include "SceneData.h"

namespace Boolka
{
    class FileReader;

    class [[nodiscard]] SceneDataReader
    {
    public:
        SceneDataReader();
        ~SceneDataReader();

        bool OpenScene(Device& device, const wchar_t* folderPath);
        void CloseReader();

        struct [[nodiscard]] HeaderWrapper
        {
            const SceneData::SceneHeader* header;
            const SceneData::TextureHeader* textureHeaders;
            const SceneData::CPUObjectHeader* cpuObjectHeaders;
        };

        HeaderWrapper GetHeaderWrapper();
        DStorageFile& GetSceneDataFile();

    private:
        struct [[nodiscard]] HeaderStart
        {
            SceneData::FormatHeader formatHeader;
            SceneData::SceneHeader sceneHeader;
        };

        MemoryBlock m_Header;
        DStorageFile m_SceneDataFile;
    };

} // namespace Boolka
