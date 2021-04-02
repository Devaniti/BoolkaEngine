#include "stdafx.h"

#include "OBJConverter.h"

#include <DirectXMath.h>
#include <d3d12.h>

#include "BoolkaCommon/DebugHelpers/DebugFileWriter.h"
#include "BoolkaCommon/DebugHelpers/DebugTimer.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"
#include "ThirdParty/DirectXMesh/DirectXMesh/DirectXMesh.h"
#include "tinyobjloader/tiny_obj_loader.h"

namespace Boolka
{
    static const size_t gs_ResourceAlignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
    static const size_t gs_PitchAlignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;

    class ObjConverterImpl
    {
    public:
        ObjConverterImpl();
        ~ObjConverterImpl() = default;

        bool Convert(std::wstring inFile, std::wstring outFile);

    private:
        void Reset();

        struct UniqueVertexKey
        {
            int vertexIndex;
            int normalIndex;
            int texcoordIndex;

            bool operator<(const UniqueVertexKey& other) const;
        };

        bool Load(std::wstring inFile);

        void ProcessGeometry();

        void RemapMaterials();

        bool IsTransparent(const tinyobj::material_t& material);

        void ProcessVerticesIndices();
        void RemapVertices(std::map<UniqueVertexKey, uint32_t>& verticesMap);

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

        std::vector<SceneData::VertexData1> m_vertexData1;
        std::vector<SceneData::VertexData2> m_vertexData2;
        // Contains uint32_t data, but declared as uint8_t since DirectXMesh requires uint8_t vector
        std::vector<uint8_t> m_vertexIndirection;
        std::vector<DirectX::MeshletTriangle> m_indexData;
        std::vector<SceneData::MeshletData> m_meshlets;
        std::vector<SceneData::ObjectHeader> m_objects;
        std::vector<std::string> m_remappedMaterials;
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

        m_vertexData1.clear();
        m_vertexData2.clear();
        m_indexData.clear();
        m_remappedMaterials.clear();
        m_objects.clear();

        m_materialsMap.clear();
    }

    bool ObjConverterImpl::Convert(std::wstring inFile, std::wstring outFile)
    {
        std::wcout << "Loading file:" << inFile << std::endl;

        if (!Load(inFile))
        {
            std::wcout << "Failed to load file:" << inFile << std::endl;
            return false;
        }

        std::wcout << inFile << " Loaded successfully" << std::endl;
        m_opaqueObjectCount = 0;

        ProcessGeometry();

        DebugFileWriter fileWriter;
        bool res = fileWriter.OpenFile(outFile.c_str());
        if (!res)
        {
            std::wcout << "Failed to open file " << outFile << " for writing" << std::endl;
            return false;
        }

        WriteHeader(fileWriter);

        WriteTextureHeaders(fileWriter);

        WriteVector(fileWriter, m_vertexData1, gs_ResourceAlignment);
        std::cout << "Written vertex buffer 1" << std::endl;

        WriteVector(fileWriter, m_vertexData2, gs_ResourceAlignment);
        std::cout << "Written vertex buffer 2" << std::endl;

        WriteVector(fileWriter, m_vertexIndirection, gs_ResourceAlignment);
        std::cout << "Written vertex indirection buffer" << std::endl;

        WriteVector(fileWriter, m_indexData, gs_ResourceAlignment);
        std::cout << "Written index buffer" << std::endl;

        WriteVector(fileWriter, m_meshlets, gs_ResourceAlignment);
        std::cout << "Written meshlets buffer" << std::endl;

        WriteVector(fileWriter, m_objects, gs_ResourceAlignment);
        std::cout << "Written objects buffer" << std::endl;

        WriteTextures(fileWriter);
        std::cout << "Written textures" << std::endl;

        res = fileWriter.Close(BLK_FILE_BLOCK_SIZE);
        BLK_CRITICAL_ASSERT(res);

        if (!res)
        {
            std::cout << "Failed to close file" << std::endl;
            return false;
        }

        std::wcout << "Successfully written " << outFile << std::endl;

        return true;

        Reset();
    }

    bool ObjConverterImpl::Load(std::wstring inFile)
    {
        std::string warn;
        std::string err;

        std::string inFileA = utf8_encode(inFile);

        DebugTimer timer;
        timer.Start();
        bool ret = tinyobj::LoadObj(&m_attrib, &m_shapes, &m_materials, &warn, &err,
                                    inFileA.c_str(), NULL, true);
        float seconds = timer.Stop();
        std::cout << "tinyobj::LoadObj tooks " << seconds << "s" << std::endl;

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
        m_materialsMap.reserve(m_materials.size() + 1);
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

        std::map<UniqueVertexKey, uint32_t> verticesMap;
        RemapVertices(verticesMap);

        const auto& vertices = m_attrib.vertices;
        std::vector<DirectX::XMFLOAT3> dxVertices;

        dxVertices.resize(verticesMap.size());

        auto verticesIter = verticesMap.begin();
        for (size_t i = 0; i < dxVertices.size(); ++i)
        {
            const auto& remappedVertex = *verticesIter;
            int posIndex = remappedVertex.first.vertexIndex;
            dxVertices[i] = DirectX::XMFLOAT3{vertices[3 * posIndex], vertices[3 * posIndex + 1],
                                              vertices[3 * posIndex + 2]};
        }

        const size_t nVerts = dxVertices.size();

        for (size_t processTransparent = 0; processTransparent < 2; ++processTransparent)
        {
            for (auto& shape : m_shapes)
            {
                SceneData::ObjectHeader object{};
                object.meshletOffset = static_cast<uint32_t>(m_meshlets.size());

                const auto& material = m_materials[shape.mesh.material_ids[0]];

                bool isCurrentTransparent = IsTransparent(material);

                if (static_cast<bool>(processTransparent) != isCurrentTransparent)
                {
                    continue;
                }

                int materialIndex = m_materialsMap[material.diffuse_texname];
                object.materialIndex = materialIndex;

                std::vector<DirectX::Meshlet> meshlets;

                const auto& indices = shape.mesh.indices;
                BLK_CRITICAL_ASSERT(indices.size() % 3 == 0);
                size_t nFaces = indices.size() / 3;

                std::vector<uint32_t> dxIndices(indices.size());
                for (size_t i = 0; i < dxIndices.size(); ++i)
                {
                    const auto& index = indices[i];
                    UniqueVertexKey remappedVertexKey = {index.vertex_index, index.normal_index,
                                                         index.texcoord_index};
                    dxIndices[i] = verticesMap[remappedVertexKey];
                }

                HRESULT hr = DirectX::ComputeMeshlets(
                    dxIndices.data(), nFaces, dxVertices.data(), nVerts, nullptr, meshlets,
                    m_vertexIndirection, m_indexData, BLK_MESHLET_MAX_VERTS, BLK_MESHLET_MAX_PRIMS);

                BLK_ASSERT_VAR2(SUCCEEDED(hr), hr);

                object.meshletCount = static_cast<uint32_t>(meshlets.size());

                for (const auto& meshlet : meshlets)
                {
                    SceneData::MeshletData meshletData{};

                    meshletData.VertCount = meshlet.VertCount;
                    meshletData.VertOffset = meshlet.VertOffset;
                    meshletData.PrimCount = meshlet.PrimCount;
                    meshletData.PrimOffset = meshlet.PrimOffset;
                    meshletData.CullingData = {};

                    m_meshlets.push_back(meshletData);
                }

                m_objects.push_back(object);

                std::cout << "processed meshlets for shape: " << shape.name << std::endl;
            }

            if (processTransparent == 0)
            {
                m_opaqueObjectCount = m_objects.size();
            }
        }
    }

    void ObjConverterImpl::RemapVertices(std::map<UniqueVertexKey, uint32_t>& verticesMap)
    {
        for (auto& shape : m_shapes)
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
                verticesMap.insert(std::pair(
                    UniqueVertexKey{index.vertex_index, index.normal_index, index.texcoord_index},
                    0));
            }

            for (unsigned char vertexesPerFace : shape.mesh.num_face_vertices)
            {
                // Further code assume that there are only triangles
                BLK_ASSERT_VAR2(vertexesPerFace == 3, vertexesPerFace);
            }

            std::cout << "Added vertices from shape: " << shape.name << std::endl;
        }

        auto& positions = m_attrib.vertices;
        auto& normals = m_attrib.normals;
        auto& texcoords = m_attrib.texcoords;

        uint32_t vertexIndex = 0;
        for (auto& [uniqueVertex, arrayIndex] : verticesMap)
        {
            SceneData::VertexData1 vertexData1{};
            SceneData::VertexData2 vertexData2{};

            float empty[3] = {};

            if (uniqueVertex.vertexIndex >= 0)
            {
                // Swap y and z
                // In OBJ y is up, and in Boolka Engine z is up
                vertexData1.position[0] = positions[3ll * uniqueVertex.vertexIndex];
                vertexData1.position[1] = positions[3ll * uniqueVertex.vertexIndex + 2];
                vertexData1.position[2] = positions[3ll * uniqueVertex.vertexIndex + 1];
            }
            else
            {
                memcpy(vertexData1.position, empty, sizeof(vertexData1.position));
            }

            if (uniqueVertex.normalIndex >= 0)
            {
                // Swap y and z
                // In OBJ y is up, and in Boolka Engine z is up
                vertexData2.normal[0] = normals[3ll * uniqueVertex.normalIndex];
                vertexData2.normal[1] = normals[3ll * uniqueVertex.normalIndex + 2];
                vertexData2.normal[2] = normals[3ll * uniqueVertex.normalIndex + 1];
            }
            else
            {
                memcpy(vertexData2.normal, empty, sizeof(vertexData2.normal));
            }

            if (uniqueVertex.texcoordIndex >= 0)
            {
                // Flip y coordinate
                // In obj, y = 0 is bottom and in DirectX y = 0 is top
                vertexData1.textureCoordX = texcoords[2ll * uniqueVertex.texcoordIndex];
                vertexData2.textureCoordY = 1 - texcoords[2ll * uniqueVertex.texcoordIndex + 1];
            }
            else
            {
                vertexData1.textureCoordX = 0.0f;
                vertexData2.textureCoordY = 0.0f;
            }

            m_vertexData1.push_back(vertexData1);
            m_vertexData2.push_back(vertexData2);
            arrayIndex = vertexIndex++;
        }

        std::cout << "Processed vertices" << std::endl;
    }

    void ObjConverterImpl::WriteHeader(DebugFileWriter& fileWriter)
    {
        SceneData::SceneHeader header{
            checked_narrowing_cast<UINT>(BLK_CEIL_TO_POWER_OF_TWO(
                m_vertexData1.size() * sizeof(m_vertexData1[0]), gs_ResourceAlignment)),
            checked_narrowing_cast<UINT>(BLK_CEIL_TO_POWER_OF_TWO(
                m_vertexData2.size() * sizeof(m_vertexData2[0]), gs_ResourceAlignment)),
            checked_narrowing_cast<UINT>(BLK_CEIL_TO_POWER_OF_TWO(
                m_vertexIndirection.size() * sizeof(m_vertexIndirection[0]), gs_ResourceAlignment)),
            checked_narrowing_cast<UINT>(BLK_CEIL_TO_POWER_OF_TWO(
                m_indexData.size() * sizeof(m_indexData[0]), gs_ResourceAlignment)),
            checked_narrowing_cast<UINT>(BLK_CEIL_TO_POWER_OF_TWO(
                m_meshlets.size() * sizeof(m_meshlets[0]), gs_ResourceAlignment)),
            checked_narrowing_cast<UINT>(BLK_CEIL_TO_POWER_OF_TWO(
                m_objects.size() * sizeof(SceneData::ObjectHeader), gs_ResourceAlignment)),
            checked_narrowing_cast<UINT>(m_objects.size()),
            checked_narrowing_cast<UINT>(m_opaqueObjectCount),
            checked_narrowing_cast<UINT>(m_remappedMaterials.size())};

        BLK_CRITICAL_ASSERT(header.vertex1Size != 0);
        BLK_CRITICAL_ASSERT(header.vertex2Size != 0);
        BLK_CRITICAL_ASSERT(header.vertexIndirectionSize != 0);
        BLK_CRITICAL_ASSERT(header.indexSize != 0);
        BLK_CRITICAL_ASSERT(header.meshletsSize != 0);
        BLK_CRITICAL_ASSERT(header.objectsSize != 0);
        BLK_CRITICAL_ASSERT(header.objectCount != 0);
        BLK_CRITICAL_ASSERT(header.opaqueCount != 0);
        BLK_CRITICAL_ASSERT(header.textureCount != 0);

        bool res = fileWriter.Write(&header, sizeof(header));
        BLK_ASSERT_VAR(res);

        std::cout << "Written header" << std::endl;
    }

    void ObjConverterImpl::WriteTextureHeaders(DebugFileWriter& fileWriter)
    {
        // First element is always empty texture
        BLK_CRITICAL_ASSERT(m_remappedMaterials.size() > 0);
        BLK_CRITICAL_ASSERT(m_remappedMaterials[0].empty());

        SceneData::TextureHeader firstHeader{1, 1, 1};
        fileWriter.Write(&firstHeader, sizeof(firstHeader));

        for (size_t i = 1; i < m_remappedMaterials.size(); ++i)
        {
            auto& material = m_remappedMaterials[i];

            BLK_CRITICAL_ASSERT(!material.empty())

            int width, height, dummy;

            int result = stbi_info(material.c_str(), &width, &height, &dummy);
            if (result == 0)
            {
                throw std::runtime_error("Can't get texture info");
            }

            UINT mipCount = 0;
            int dimension = std::min(width, height);
            while (dimension)
            {
                mipCount++;
                dimension >>= 1;
            }

            SceneData::TextureHeader textureHeader{checked_narrowing_cast<UINT>(width),
                                                   checked_narrowing_cast<UINT>(height), mipCount};

            BLK_CRITICAL_ASSERT(result != 0);

            fileWriter.Write(&textureHeader, sizeof(textureHeader));

            std::cout << "texture " << i << " loaded\n";
        }

        std::cout << "Written texture headers" << std::endl;
    }

    template <typename T>
    void ObjConverterImpl::WriteVector(DebugFileWriter& fileWriter,
                                       const std::vector<T>& dataVector, size_t alignment)
    {
        size_t size = dataVector.size() * sizeof(T);
        bool res = fileWriter.Write(dataVector.data(), size);
        BLK_ASSERT_VAR(res);
        if (alignment > 0)
        {
            size_t modulo = size % alignment;
            if (modulo > 0)
            {
                fileWriter.AddPadding(alignment - modulo);
            }
        }
    }

    void ObjConverterImpl::WriteTextures(DebugFileWriter& fileWriter)
    {
        // First element is always empty texture
        BLK_CRITICAL_ASSERT(m_remappedMaterials.size() > 0);
        BLK_CRITICAL_ASSERT(m_remappedMaterials[0].empty());

        // First empty texture
        unsigned char pixel[4] = {};
        WriteMIPChain(fileWriter, pixel, 1, 1);

        for (size_t i = 1; i < m_remappedMaterials.size(); ++i)
        {
            auto& material = m_remappedMaterials[i];

            BLK_CRITICAL_ASSERT(!material.empty())

            SceneData::TextureHeader textureHeader{};

            if (material.empty())
            {
                fileWriter.Write(&textureHeader, sizeof(textureHeader));
                continue;
            }

            int width, height, bitsPerPixel;

            unsigned char* textureData =
                stbi_load(material.c_str(), &width, &height, &bitsPerPixel, 4);

            BLK_CRITICAL_ASSERT(textureData);

            WriteMIPChain(fileWriter, textureData, width, height);

            stbi_image_free(textureData);

            std::cout << "texture " << i << ":" << material << " written" << std::endl;
        }
    }

    void ObjConverterImpl::WriteMIPChain(DebugFileWriter& fileWriter,
                                         const unsigned char* textureData, int width, int height)
    {
        BLK_CRITICAL_ASSERT(width > 0);
        BLK_CRITICAL_ASSERT(height > 0);

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
                    unsigned char rgba[ms_bytesPerPixel]{};
                    unsigned char prevMipRGBA[4][ms_bytesPerPixel]{};

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

    bool OBJConverter::Convert(std::wstring inFile, std::wstring outFile)
    {
        ObjConverterImpl converter;
        return converter.Convert(inFile, outFile);
    }

} // namespace Boolka
