#include "stdafx.h"

#include "ObjConverter.h"

#include "BoolkaCommon/DebugHelpers/DebugFileWriter.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"
#include "tinyobjloader/tiny_obj_loader.h"

namespace Boolka
{
    static const size_t gs_ResourceAlignment = 512;
    static const size_t gs_PitchAlignment = 256;

    class ObjConverterImpl
    {
    public:
        ObjConverterImpl();
        ~ObjConverterImpl() = default;

        bool Convert(std::string inFile, std::string outFile);

    private:
        void Reset();

        struct VertexData
        {
            float position[3];
            int materialId;
            float normal[3];
            float textureCoords[2];
        };

        struct UniqueVertexKey
        {
            int materialIndex;
            int vertexIndex;
            int normalIndex;
            int texcoordIndex;

            bool operator<(const UniqueVertexKey& other) const;
        };

        bool Load(std::string inFile);

        void ProcessGeometry();

        void RemapMaterials();

        bool IsTransparent(const tinyobj::material_t& material);

        void ProcessVerticesIndices();

        void RemapVertices(const tinyobj::shape_t& shape,
                           std::map<UniqueVertexKey, uint32_t>& verticesMap,
                           uint32_t& currentIndex);

        void BuildIndices(const tinyobj::shape_t& shape,
                          std::map<UniqueVertexKey, uint32_t>& verticesMap,
                          SceneData::ObjectHeader& object);

        void WriteHeader(DebugFileWriter& fileWriter);

        template <typename T>
        void WriteVector(DebugFileWriter& fileWriter, const std::vector<T>& vertexDataVector,
                         size_t alignment);
        void WriteTextureHeaders(DebugFileWriter& fileWriter);
        void WriteTextures(DebugFileWriter& fileWriter);
        void WriteMIPChain(DebugFileWriter& fileWriter, const unsigned char* textureData, int width,
                           int height);

        static const size_t ms_bytesPerPixel = 4;

        tinyobj::attrib_t m_attrib;
        std::vector<tinyobj::shape_t> m_shapes;
        std::vector<tinyobj::material_t> m_materials;

        std::vector<VertexData> m_vertexDataVector;
        std::vector<uint32_t> m_indexDataVector;
        std::vector<std::string> m_remappedMaterials;
        std::vector<SceneData::ObjectHeader> m_objects;
        size_t m_opaqueObjectCount;

        std::unordered_map<std::string, int> m_materialsMap;
    };

    ObjConverterImpl::ObjConverterImpl()
    {
        Reset();
    }

    void ObjConverterImpl::Reset()
    {
        m_opaqueObjectCount = 0;

        m_shapes.clear();
        m_materials.clear();

        m_vertexDataVector.clear();
        m_indexDataVector.clear();
        m_remappedMaterials.clear();
        m_objects.clear();

        m_materialsMap.clear();
    }

    bool ObjConverterImpl::Convert(std::string inFile, std::string outFile)
    {
        std::cout << "Loading file:" << inFile << std::endl;

        if (!Load(inFile))
        {
            std::cout << "Failed to load file:" << inFile << std::endl;
            return false;
        }

        std::cout << inFile << "Loaded successfully" << std::endl;
        m_opaqueObjectCount = 0;

        ProcessGeometry();

        DebugFileWriter fileWriter;
        bool res = fileWriter.OpenFile(outFile.c_str());
        if (!res)
        {
            std::cout << "Failed to open file " << outFile << " for writing" << std::endl;
            return false;
        }

        WriteHeader(fileWriter);

        WriteTextureHeaders(fileWriter);

        WriteVector(fileWriter, m_objects, 0);
        std::cout << "Written objects buffer" << std::endl;

        WriteVector(fileWriter, m_vertexDataVector, gs_ResourceAlignment);
        std::cout << "Written vertex buffer" << std::endl;

        WriteVector(fileWriter, m_indexDataVector, gs_ResourceAlignment);
        std::cout << "Written index buffer" << std::endl;

        WriteTextures(fileWriter);
        std::cout << "Written textures" << std::endl;

        res = fileWriter.Close(BLK_FILE_BLOCK_SIZE);
        BLK_ASSERT(res);

        if (!res)
        {
            std::cout << "Failed to close file" << std::endl;
            return false;
        }

        std::cout << "Successfully written " << outFile << std::endl;

        return true;

        Reset();
    }

    bool ObjConverterImpl::Load(std::string inFile)
    {
        std::string warn;
        std::string err;

        bool ret = tinyobj::LoadObj(&m_attrib, &m_shapes, &m_materials, &warn, &err, inFile.c_str(),
                                    NULL, true);

        if (!warn.empty())
        {
            std::cerr << "tinyobj warning:\n" << warn;
        }

        if (!err.empty())
        {
            std::cerr << "tinyobj error:\n" << err;
        }

        return ret;
    }

    void ObjConverterImpl::ProcessGeometry()
    {
        RemapMaterials();
        ProcessVerticesIndices();
    }
    void ObjConverterImpl::RemapMaterials()
    {
        m_materialsMap.insert(std::make_pair<std::string, int>("", 0));
        int currentMaterialIndex = 1;

        for (auto& material : m_materials)
        {
            if (m_materialsMap.find(material.diffuse_texname) == m_materialsMap.end())
            {
                m_materialsMap[material.diffuse_texname] = currentMaterialIndex++;
            }
        }

        m_remappedMaterials.resize(m_materialsMap.size());
        for (auto& [textureName, materialIndex] : m_materialsMap)
        {
            m_remappedMaterials[materialIndex] = textureName;
        }

        std::cout << "Remapped materials" << std::endl;
    }

    // TODO find better way to determine whether material have transparency
    bool ObjConverterImpl::IsTransparent(const tinyobj::material_t& material)
    {
        if (material.illum > 2)
            return true;

        const auto& filename = material.diffuse_texname;

        if (filename.empty())
        {
            return false;
        }

        static std::unordered_map<std::string, bool> s_calculated;

        auto iter = s_calculated.find(filename);
        if (iter != s_calculated.end())
        {
            return iter->second;
        }

        int width, height, bitsPerPixel;
        unsigned char* textureData = stbi_load(filename.c_str(), &width, &height, &bitsPerPixel, 4);

        bool result = false;

        for (size_t pixel = 0; pixel < static_cast<size_t>(width) * height; ++pixel)
        {
            if (textureData[pixel * 4 + 3] != 255)
            {
                result = true;
                break;
            }
        }

        stbi_image_free(textureData);

        s_calculated[filename] = result;
        return result;
    }

    void ObjConverterImpl::ProcessVerticesIndices()
    {
        uint32_t currentIndex = 0;

        m_objects.reserve(m_shapes.size());

        for (size_t processTransparent = 0; processTransparent < 2; ++processTransparent)
        {
            for (auto& shape : m_shapes)
            {
                SceneData::ObjectHeader object{};

                auto& material = m_materials[shape.mesh.material_ids[0]];

                bool isCurrentTransparent = IsTransparent(material);

                if (static_cast<bool>(processTransparent) != isCurrentTransparent)
                {
                    continue;
                }

                std::map<UniqueVertexKey, uint32_t> verticesMap;

                RemapVertices(shape, verticesMap, currentIndex);
                BuildIndices(shape, verticesMap, object);

                m_objects.push_back(object);
            }

            if (processTransparent == 0)
            {
                m_opaqueObjectCount = m_objects.size();
            }
        }
    }

    void ObjConverterImpl::RemapVertices(const tinyobj::shape_t& shape,
                                         std::map<UniqueVertexKey, uint32_t>& verticesMap,
                                         uint32_t& currentIndex)
    {
        for (unsigned char vertexesPerFace : shape.mesh.num_face_vertices)
        {
            // Further code assume that there are only triangles
            BLK_ASSERT(vertexesPerFace == 3);
        }

        auto& mesh = shape.mesh;
        auto& indices = mesh.indices;
        for (size_t i = 0; i < indices.size(); i++)
        {
            auto& index = indices[i];
            size_t triangleIndex = i / 3;
            int materialIndex = mesh.material_ids[triangleIndex];
            verticesMap.insert(std::pair(UniqueVertexKey{materialIndex, index.vertex_index,
                                                         index.normal_index, index.texcoord_index},
                                         0));
        }

        auto& positions = m_attrib.vertices;
        auto& normals = m_attrib.normals;
        auto& texcoords = m_attrib.texcoords;

        uint32_t i = 0;
        for (auto& [uniqueVertex, arrayIndex] : verticesMap)
        {
            VertexData vertexData;

            float empty[3] = {};

            if (uniqueVertex.vertexIndex >= 0)
            {
                // Swap y and z
                // In OBJ y is up, and in Boolka Engine z is up
                vertexData.position[0] = positions[3 * uniqueVertex.vertexIndex];
                vertexData.position[1] = positions[3 * uniqueVertex.vertexIndex + 2];
                vertexData.position[2] = positions[3 * uniqueVertex.vertexIndex + 1];
            }
            else
            {
                memcpy(vertexData.position, empty, sizeof(vertexData.position));
            }

            vertexData.materialId =
                m_materialsMap[m_materials[uniqueVertex.materialIndex].diffuse_texname];

            if (uniqueVertex.normalIndex >= 0)
            {
                // Swap y and z
                // In OBJ y is up, and in Boolka Engine z is up
                vertexData.normal[0] = normals[3 * uniqueVertex.normalIndex];
                vertexData.normal[1] = normals[3 * uniqueVertex.normalIndex + 2];
                vertexData.normal[2] = normals[3 * uniqueVertex.normalIndex + 1];
            }
            else
            {
                memcpy(vertexData.normal, empty, sizeof(vertexData.normal));
            }

            if (uniqueVertex.texcoordIndex >= 0)
            {
                // Flip y coordinate
                // In obj, y = 0 is bottom and in DirectX y = 0 is top
                vertexData.textureCoords[0] = texcoords[2 * uniqueVertex.texcoordIndex];
                vertexData.textureCoords[1] = 1 - texcoords[2 * uniqueVertex.texcoordIndex + 1];
            }
            else
            {
                memcpy(vertexData.textureCoords, empty, sizeof(vertexData.textureCoords));
            }

            m_vertexDataVector.push_back(vertexData);
            arrayIndex = currentIndex++;
        }

        std::cout << "Processed vertices for shape " << shape.name << std::endl;
    }

    void ObjConverterImpl::BuildIndices(const tinyobj::shape_t& shape,
                                        std::map<UniqueVertexKey, uint32_t>& verticesMap,
                                        SceneData::ObjectHeader& object)
    {
        auto& mesh = shape.mesh;
        auto& indices = mesh.indices;

        object.indexCount = checked_narrowing_cast<UINT>(indices.size());
        object.startIndex = checked_narrowing_cast<UINT>(m_indexDataVector.size());
        object.boundingBox.GetMax() = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
        object.boundingBox.GetMin() = {FLT_MAX, FLT_MAX, FLT_MAX};

        auto& positions = m_attrib.vertices;

        for (size_t i = 0; i < indices.size(); i++)
        {
            auto& index = indices[i];
            size_t triangleIndex = i / 3;
            int materialIndex = mesh.material_ids[triangleIndex];
            m_indexDataVector.push_back(verticesMap[UniqueVertexKey{
                materialIndex, index.vertex_index, index.normal_index, index.texcoord_index}]);

            int vertexIndex = index.vertex_index;
            Vector3 xyz = {positions[3 * vertexIndex], positions[3 * vertexIndex + 2],
                           positions[3 * vertexIndex + 1]};
            object.boundingBox.GetMax() = Max(object.boundingBox.GetMax(), xyz);
            object.boundingBox.GetMin() = Min(object.boundingBox.GetMin(), xyz);
        }

        std::cout << "Processed indices for shape " << shape.name << std::endl;
    }

    void ObjConverterImpl::WriteHeader(DebugFileWriter& fileWriter)
    {
        SceneData::SceneHeader header{
            checked_narrowing_cast<UINT>(BLK_CEIL_TO_POWER_OF_TWO(
                m_vertexDataVector.size() * sizeof(VertexData), gs_ResourceAlignment)),
            checked_narrowing_cast<UINT>(BLK_CEIL_TO_POWER_OF_TWO(
                m_indexDataVector.size() * sizeof(uint32_t), gs_ResourceAlignment)),
            checked_narrowing_cast<UINT>(m_objects.size() * sizeof(SceneData::ObjectHeader)),
            checked_narrowing_cast<UINT>(m_indexDataVector.size()),
            checked_narrowing_cast<UINT>(m_objects.size()),
            checked_narrowing_cast<UINT>(m_opaqueObjectCount),
            checked_narrowing_cast<UINT>(m_remappedMaterials.size())};

        bool res = fileWriter.Write(&header, sizeof(header));
        BLK_ASSERT(res);

        std::cout << "Written header" << std::endl;
    }

    void ObjConverterImpl::WriteTextureHeaders(DebugFileWriter& fileWriter)
    {
        // First element is always empty texture
        BLK_ASSERT(m_remappedMaterials.size() > 0);
        BLK_ASSERT(m_remappedMaterials[0].empty());

        SceneData::TextureHeader firstHeader{1, 1, 1};
        fileWriter.Write(&firstHeader, sizeof(firstHeader));

        for (size_t i = 1; i < m_remappedMaterials.size(); ++i)
        {
            auto& material = m_remappedMaterials[i];

            BLK_ASSERT(!material.empty())

            int width, height, dummy;

            int result = stbi_info(material.c_str(), &width, &height, &dummy);

            UINT mipCount = 0;
            int dimension = min(width, height);
            while (dimension)
            {
                mipCount++;
                dimension >>= 1;
            }

            SceneData::TextureHeader textureHeader{checked_narrowing_cast<UINT>(width),
                                                   checked_narrowing_cast<UINT>(height), mipCount};

            BLK_ASSERT(result != 0);

            fileWriter.Write(&textureHeader, sizeof(textureHeader));

            std::cout << "texture " << i << " loaded\n";
        }

        std::cout << "Written texture headers" << std::endl;
    }

    template <typename T>
    void ObjConverterImpl::WriteVector(DebugFileWriter& fileWriter,
                                       const std::vector<T>& vertexDataVector, size_t alignment)
    {
        size_t size = vertexDataVector.size() * sizeof(T);
        bool res = fileWriter.Write(vertexDataVector.data(), size);
        if (alignment > 0)
        {
            size_t modulo = size % alignment;
            if (modulo > 0)
            {
                fileWriter.AddPadding(alignment - modulo);
            }
        }

        BLK_ASSERT(res);
    }

    void ObjConverterImpl::WriteTextures(DebugFileWriter& fileWriter)
    {
        // First element is always empty texture
        BLK_ASSERT(m_remappedMaterials.size() > 0);
        BLK_ASSERT(m_remappedMaterials[0].empty());

        // First empty texture
        unsigned char pixel[4] = {};
        WriteMIPChain(fileWriter, pixel, 1, 1);

        for (size_t i = 1; i < m_remappedMaterials.size(); ++i)
        {
            auto& material = m_remappedMaterials[i];

            BLK_ASSERT(!material.empty())

            SceneData::TextureHeader textureHeader{};

            if (material.empty())
            {
                fileWriter.Write(&textureHeader, sizeof(textureHeader));
                continue;
            }

            int width, height, bitsPerPixel;

            unsigned char* textureData =
                stbi_load(material.c_str(), &width, &height, &bitsPerPixel, 4);

            BLK_ASSERT(textureData);

            WriteMIPChain(fileWriter, textureData, width, height);

            stbi_image_free(textureData);

            std::cout << "texture " << i << ":" << material << " written" << std::endl;
        }
    }

    void ObjConverterImpl::WriteMIPChain(DebugFileWriter& fileWriter,
                                         const unsigned char* textureData, int width, int height)
    {
        BLK_ASSERT(width > 0);
        BLK_ASSERT(height > 0);

        size_t srcRowPitch = ms_bytesPerPixel * width;
        size_t mip0RowPitch = BLK_CEIL_TO_POWER_OF_TWO(ms_bytesPerPixel * width, gs_PitchAlignment);
        size_t mip0Size = BLK_CEIL_TO_POWER_OF_TWO(mip0RowPitch * height, gs_ResourceAlignment);

        unsigned char* mip0Data = new unsigned char[mip0Size];

        memcpy_strided(mip0Data, mip0RowPitch, textureData, srcRowPitch, height);

        fileWriter.Write(mip0Data, mip0Size);

        const unsigned char* prevMipData = mip0Data;
        size_t prevMipRowPitch = mip0RowPitch;

        for (size_t mipWidth = width / 2, mipHeight = height / 2; mipWidth > 0 && mipHeight > 0;
             mipWidth /= 2, mipHeight /= 2)
        {
            size_t rowPitch =
                BLK_CEIL_TO_POWER_OF_TWO(ms_bytesPerPixel * mipWidth, gs_PitchAlignment);
            size_t mipSize = BLK_CEIL_TO_POWER_OF_TWO(rowPitch * mipHeight, gs_ResourceAlignment);
            unsigned char* mipData = new unsigned char[mipSize];

            for (size_t y = 0; y < mipHeight; ++y)
            {
                for (size_t x = 0; x < mipWidth; ++x)
                {
                    unsigned char rgba[ms_bytesPerPixel];
                    unsigned char prevMipRGBA[4][ms_bytesPerPixel];

                    for (size_t i = 0; i < 2; ++i)
                    {
                        for (size_t j = 0; j < 2; ++j)
                        {
                            memcpy(prevMipRGBA[2 * i + j],
                                   &prevMipData[prevMipRowPitch * (y * 2 + i) +
                                                ms_bytesPerPixel * (x * 2 + j)],
                                   ms_bytesPerPixel);
                        }
                    }

                    for (size_t i = 0; i < 4; ++i)
                    {
                        int sum = 0;
                        for (auto& data : prevMipRGBA)
                        {
                            sum += data[i];
                        }
                        rgba[i] = sum / 4;
                    }

                    memcpy(&mipData[rowPitch * y + ms_bytesPerPixel * x], rgba, ms_bytesPerPixel);
                }
            }

            delete[] prevMipData;
            prevMipData = mipData;
            prevMipRowPitch = rowPitch;

            fileWriter.Write(mipData, mipSize);
        }
    }

    bool ObjConverterImpl::UniqueVertexKey::operator<(const UniqueVertexKey& other) const
    {
        if (materialIndex != other.materialIndex)
        {
            return materialIndex < other.materialIndex;
        }

        if (vertexIndex != other.vertexIndex)
        {
            return vertexIndex < other.vertexIndex;
        }

        if (normalIndex != other.normalIndex)
        {
            return normalIndex < other.normalIndex;
        }

        return texcoordIndex < other.texcoordIndex;
    }

    bool OBJConverter::Convert(std::string inFile, std::string outFile)
    {
        ObjConverterImpl converter;
        return converter.Convert(inFile, outFile);
    }

} // namespace Boolka
