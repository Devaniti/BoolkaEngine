#include "stdafx.h"

#include "ObjConverter.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"
#include "BoolkaCommon/DebugHelpers/DebugFileWriter.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"

namespace Boolka
{

    class ObjConverterImpl
    {
    public:
        static bool Convert(std::string inFile, std::string outFile);

    private:
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

        static bool Load(std::string inFile,
            tinyobj::attrib_t& attrib,
            std::vector<tinyobj::shape_t>& shapes,
            std::vector<tinyobj::material_t>& materials);

        static void ProcessGeometry(const tinyobj::attrib_t& attrib,
            const std::vector<tinyobj::shape_t>& shapes,
            const std::vector<tinyobj::material_t>& materials,
            std::vector<VertexData>& vertexDataVector,
            std::vector<uint32_t>& indexDataVector,
            std::vector<std::string>& remappedMaterials);

        static void RemapMaterials(const std::vector<tinyobj::material_t>& materials,
            std::unordered_map<std::string, int>& materialsMap,
            std::vector<std::string>& remappedMaterials);

        static void ProcessVerticesIndices(const tinyobj::attrib_t& attrib,
            const std::vector<tinyobj::shape_t>& shapes,
            const std::vector<tinyobj::material_t>& materials,
            std::unordered_map<std::string, int>& materialsMap,
            std::vector<VertexData>& vertexDataVector,
            std::vector<uint32_t>& indexDataVector);

        static void RemapVertices(const tinyobj::attrib_t& attrib,
            const std::vector<tinyobj::shape_t>& shapes,
            const std::vector<tinyobj::material_t>& materials,
            std::unordered_map<std::string, int>& materialsMap,
            std::map<UniqueVertexKey, uint32_t>& verticesMap,
            std::vector<VertexData>& vertexDataVector);

        static void BuildIndices(const std::vector<tinyobj::shape_t>& shapes,
            std::map<UniqueVertexKey, uint32_t>& verticesMap,
            std::vector<uint32_t>& indexDataVector);

        static void WriteHeader(DebugFileWriter& fileWriter,
            const std::vector<VertexData>& vertexDataVector,
            const std::vector<uint32_t>& indexDataVector,
            const std::vector<std::string>& remappedMaterials);

        static void WriteTextureHeaders(DebugFileWriter& fileWriter, std::vector<std::string>& remappedMaterials);
        static void WriteVertexBuffer(DebugFileWriter& fileWriter, const std::vector<VertexData>& vertexDataVector);
        static void WriteIndexBuffer(DebugFileWriter& fileWriter, const std::vector<uint32_t>& indexDataVector);
        static void WriteTextures(DebugFileWriter& fileWriter, std::vector<std::string>& remappedMaterials);
        static void WriteMIPChain(DebugFileWriter& fileWriter, const unsigned char* textureData, int width, int height);

        static const size_t bytesPerPixel = 4;
    };

    bool ObjConverterImpl::Convert(std::string inFile, std::string outFile)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::cout << "Loading file:" << inFile << std::endl;

        if (!Load(inFile, attrib, shapes, materials))
        {
            std::cout << "Failed to load file:" << inFile << std::endl;
            return false;
        }

        std::cout << inFile << "Loaded successfully" << std::endl;

        std::vector<VertexData> vertexDataVector;
        std::vector<uint32_t> indexDataVector;
        std::vector<std::string> remappedMaterials;

        ProcessGeometry(attrib, shapes, materials, vertexDataVector, indexDataVector, remappedMaterials);

        DebugFileWriter fileWriter;
        bool res = fileWriter.OpenFile(outFile.c_str());
        if (!res)
        {
            std::cout << "Failed to open file " << outFile << " for writing" << std::endl;
            return false;
        }

        WriteHeader(fileWriter, vertexDataVector, indexDataVector, remappedMaterials);

        WriteTextureHeaders(fileWriter, remappedMaterials);

        WriteVertexBuffer(fileWriter, vertexDataVector);
        WriteIndexBuffer(fileWriter, indexDataVector);
        
        WriteTextures(fileWriter, remappedMaterials);
        res = fileWriter.Close();
        BLK_ASSERT(res);

        if (!res)
        {
            std::cout << "Failed to close file" << std::endl;
            return false;
        }

        std::cout << "Successfully written" << outFile << std::endl;

        return true;
    }

    bool ObjConverterImpl::Load(std::string inFile, tinyobj::attrib_t& attrib, std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials)
    {
        std::string warn;
        std::string err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inFile.c_str(), NULL, true);

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

    void ObjConverterImpl::ProcessGeometry(const tinyobj::attrib_t& attrib,
                                       const std::vector<tinyobj::shape_t>& shapes, 
                                       const std::vector<tinyobj::material_t>& materials,
                                       std::vector<VertexData>& vertexDataVector,
                                       std::vector<uint32_t>& indexDataVector, 
                                       std::vector<std::string>& remappedMaterials)
    {
        std::unordered_map<std::string, int> materialsMap;
        RemapMaterials(materials, materialsMap, remappedMaterials);
        ProcessVerticesIndices(attrib, shapes, materials, materialsMap, vertexDataVector, indexDataVector);
    }

    void ObjConverterImpl::RemapMaterials(const std::vector<tinyobj::material_t>& materials, 
                                      std::unordered_map<std::string, int>& materialsMap, 
                                      std::vector<std::string>& remappedMaterials)
    {
        materialsMap.insert(std::make_pair<std::string, int>("", 0));
        int currentMaterialIndex = 1;

        for (auto& material : materials)
        {
            if (materialsMap.find(material.diffuse_texname) == materialsMap.end())
            {
                materialsMap[material.diffuse_texname] = currentMaterialIndex++;
            }
        }

        remappedMaterials.resize(materialsMap.size());
        for (auto& [textureName, materialIndex] : materialsMap)
        {
            remappedMaterials[materialIndex] = textureName;
        }

        std::cout << "Remapped materials" << std::endl;
    }

    void ObjConverterImpl::ProcessVerticesIndices(const tinyobj::attrib_t& attrib, 
                                     const std::vector<tinyobj::shape_t>& shapes, 
                                     const std::vector<tinyobj::material_t>& materials,
                                     std::unordered_map<std::string, int>& materialsMap, 
                                     std::vector<VertexData>& vertexDataVector, 
                                     std::vector<uint32_t>& indexDataVector)
    {
        std::map<UniqueVertexKey, uint32_t> verticesMap;

        RemapVertices(attrib, shapes, materials, materialsMap, verticesMap, vertexDataVector);
        BuildIndices(shapes, verticesMap, indexDataVector);
    }

    void ObjConverterImpl::RemapVertices(const tinyobj::attrib_t& attrib, 
        const std::vector<tinyobj::shape_t>& shapes, 
        const std::vector<tinyobj::material_t>& materials, 
        std::unordered_map<std::string, int>& materialsMap, 
        std::map<UniqueVertexKey, uint32_t>& verticesMap,
        std::vector<VertexData>& vertexDataVector)
    {
        for (auto& shape : shapes)
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
                verticesMap.insert(std::pair(UniqueVertexKey{ materialIndex, index.vertex_index, index.normal_index, index.texcoord_index }, 0));
            }
        }
        std::cout << "Built vertices map" << std::endl;

        auto& vertices = attrib.vertices;
        auto& normals = attrib.normals;
        auto& texcoords = attrib.texcoords;
        
        uint32_t i = 0;
        for (auto& [uniqueVertex, arrayIndex] : verticesMap)
        {
            VertexData vertexData;

            float empty[3] = {};

            if (uniqueVertex.vertexIndex >= 0)
            {
                // Swap y and z
                // In OBJ y is up, and in Boolka Engine z is up
                vertexData.position[0] = vertices[3 * uniqueVertex.vertexIndex];
                vertexData.position[1] = vertices[3 * uniqueVertex.vertexIndex + 2];
                vertexData.position[2] = vertices[3 * uniqueVertex.vertexIndex + 1];
            }
            else
            {
                memcpy(vertexData.position, empty, sizeof(vertexData.position));
            }

            vertexData.materialId = materialsMap[materials[uniqueVertex.materialIndex].diffuse_texname];


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

            vertexDataVector.push_back(vertexData);
            arrayIndex = i++;
        }

        std::cout << "Processed vertices" << std::endl;
    }

    void ObjConverterImpl::BuildIndices(const std::vector<tinyobj::shape_t>& shapes, std::map<UniqueVertexKey, uint32_t>& verticesMap, std::vector<uint32_t>& indexDataVector)
    {
        for (auto& shape : shapes)
        {
            auto& mesh = shape.mesh;
            auto& indices = mesh.indices;
            for (size_t i = 0; i < indices.size(); i++)
            {
                auto& index = indices[i];
                size_t triangleIndex = i / 3;
                int materialIndex = mesh.material_ids[triangleIndex];
                indexDataVector.push_back(verticesMap[UniqueVertexKey{ materialIndex, index.vertex_index, index.normal_index, index.texcoord_index }]);
            }
        }

        std::cout << "Processed indices" << std::endl;
    }

    void ObjConverterImpl::WriteHeader(DebugFileWriter& fileWriter, const std::vector<VertexData>& vertexDataVector, const std::vector<uint32_t>& indexDataVector, const std::vector<std::string>& remappedMaterials)
    {
        SceneData::SceneHeader header
        {
            checked_narrowing_cast<UINT>(vertexDataVector.size() * sizeof(VertexData)),
            checked_narrowing_cast<UINT>(indexDataVector.size() * sizeof(uint32_t)),
            checked_narrowing_cast<int>(remappedMaterials.size())
        };

        bool res = fileWriter.Write(&header, sizeof(header));
        BLK_ASSERT(res);

        std::cout << "Written header" << std::endl;
    }

    void ObjConverterImpl::WriteTextureHeaders(DebugFileWriter& fileWriter, std::vector<std::string>& remappedMaterials)
    {
        // First element is always empty texture
        BLK_ASSERT(remappedMaterials.size() > 0);
        BLK_ASSERT(remappedMaterials[0].empty());

        SceneData::TextureHeader firstHeader{};
        fileWriter.Write(&firstHeader, sizeof(firstHeader));

        for (size_t i = 1; i < remappedMaterials.size(); ++i)
        {
            auto& material = remappedMaterials[i];

            BLK_ASSERT(!material.empty())

            int width, height, dummy;

            int result = stbi_info(material.c_str(), &width, &height, &dummy);

            SceneData::TextureHeader textureHeader
            {
                checked_narrowing_cast<UINT>(width),
                checked_narrowing_cast<UINT>(height)
            };

            BLK_ASSERT(result != 0);

            fileWriter.Write(&textureHeader, sizeof(textureHeader));

            std::cout << "texture " << i << " loaded\n";
        }

        std::cout << "Written texture headers" << std::endl;
    }

    void ObjConverterImpl::WriteVertexBuffer(DebugFileWriter& fileWriter, const std::vector<VertexData>& vertexDataVector)
    {
        bool res = fileWriter.Write(vertexDataVector.data(), vertexDataVector.size() * sizeof(VertexData));
        BLK_ASSERT(res);

        std::cout << "Written vertex buffer" << std::endl;
    }

    void ObjConverterImpl::WriteIndexBuffer(DebugFileWriter& fileWriter, const std::vector<uint32_t>& indexDataVector)
    {
        bool res = fileWriter.Write(indexDataVector.data(), indexDataVector.size() * sizeof(uint32_t));
        BLK_ASSERT(res);

        std::cout << "Written vertex buffer" << std::endl;
    }

    void ObjConverterImpl::WriteTextures(DebugFileWriter& fileWriter, std::vector<std::string>& remappedMaterials)
    {
        // First element is always empty texture
        BLK_ASSERT(remappedMaterials.size() > 0);
        BLK_ASSERT(remappedMaterials[0].empty());

        for (size_t i = 1; i < remappedMaterials.size(); ++i)
        {
            auto& material = remappedMaterials[i];

            BLK_ASSERT(!material.empty())

            SceneData::TextureHeader textureHeader{};

            if (material.empty())
            {
                fileWriter.Write(&textureHeader, sizeof(textureHeader));
                continue;
            }

            int width, height, bitsPerPixel;

            unsigned char* textureData = stbi_load(material.c_str(), &width, &height, &bitsPerPixel, 4);

            BLK_ASSERT(textureData);

            WriteMIPChain(fileWriter, textureData, width, height);

            stbi_image_free(textureData);

            std::cout << "texture " << i << ":" << material << " written" << std::endl;
        }
    }

    void ObjConverterImpl::WriteMIPChain(DebugFileWriter& fileWriter, const unsigned char* textureData, int width, int height)
    {
        BLK_ASSERT(width > 0);
        BLK_ASSERT(height > 0);

        size_t srcRowPitch = bytesPerPixel * width;
        size_t mip0RowPitch = CEIL_TO_POWER_OF_TWO(bytesPerPixel * width, 256);
        size_t mip0Size = CEIL_TO_POWER_OF_TWO(mip0RowPitch * height, 512);

        unsigned char* mip0Data = new unsigned char[mip0Size];

        memcpy_strided(mip0Data, mip0RowPitch, textureData, srcRowPitch, height);

        fileWriter.Write(mip0Data, mip0Size);

        const unsigned char* prevMipData = mip0Data;
        size_t prevMipRowPitch = mip0RowPitch;

        for (
            size_t mipWidth = width / 2, mipHeight = height / 2;
            mipWidth > 0 && mipHeight > 0;
            mipWidth /= 2, mipHeight /= 2
            )
        {
            size_t rowPitch = CEIL_TO_POWER_OF_TWO(bytesPerPixel * mipWidth, 256);
            size_t mipSize = CEIL_TO_POWER_OF_TWO(rowPitch * mipHeight, 512);
            unsigned char* mipData = new unsigned char[mipSize];

            for (size_t y = 0; y < mipHeight; ++y)
            {
                for (size_t x = 0; x < mipWidth; ++x)
                {
                    unsigned char rgba[bytesPerPixel];
                    unsigned char prevMipRGBA[4][bytesPerPixel];

                    for (size_t i = 0; i < 2; ++i)
                    {
                        for (size_t j = 0; j < 2; ++j)
                        {
                            memcpy(prevMipRGBA[2 * i + j], &prevMipData[prevMipRowPitch * (y * 2 + i) + bytesPerPixel * (x * 2 + j)], bytesPerPixel);
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

                    memcpy(&mipData[rowPitch * y + bytesPerPixel * x], rgba, bytesPerPixel);
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
        return ObjConverterImpl::Convert(inFile, outFile);
    }

}
