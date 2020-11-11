#include "stdafx.h"

#include "OBJConverter.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"
#include "BoolkaCommon/DebugHelpers/DebugFileWriter.h"
#include "D3D12Backend/Containers/Streaming/SceneData.h"

namespace Boolka
{

    OBJConverter::OBJConverter()
    {
    }

    OBJConverter::~OBJConverter()
    {
    }

    bool OBJConverter::Load(std::string filename)
    {
        bool ret = tinyobj::LoadObj(&m_attrib, &m_shapes, &m_materials, &m_warn, &m_err, filename.c_str(), NULL, true);

        if (!m_warn.empty())
        {
            std::cerr << "tinyobj warning:\n" << m_warn;
        }
        m_warn.clear();

        if (!m_err.empty())
        {
            std::cerr << "tinyobj error:\n" << m_err;
        }
        m_err.clear();

        return ret;
    }

    bool OBJConverter::Save(std::string filename)
    {
        struct VertexData
        {
            float position[3];
            int materialId;
            float normal[3];
            float textureCoords[2];
        };

        auto& vertices = m_attrib.vertices;
        auto& normals = m_attrib.normals;
        auto& texcoords = m_attrib.texcoords;

        struct UniqueVertexKey
        {
            int materialIndex;
            int vertexIndex;
            int normalIndex;
            int texcoordIndex;

            bool operator<(const UniqueVertexKey& other) const
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

                if (texcoordIndex != other.texcoordIndex)
                {
                    return texcoordIndex < other.texcoordIndex;
                }

                return false;
            }
        };

        std::map<UniqueVertexKey, uint32_t> vertexes;

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
                int materialIndex = mesh.material_ids[triangleIndex];
                vertexes.insert(std::pair(UniqueVertexKey{ materialIndex, index.vertex_index, index.normal_index, index.texcoord_index }, 0));
            }
        }

        std::cout << "loaded vertexes\n";

        std::unordered_map<std::string, int> materialsMap;

        materialsMap.insert(std::make_pair<std::string, int>("", 0));
        int currentMaterialIndex = 1;

        for (auto& material : m_materials)
        {
            if (materialsMap.find(material.diffuse_texname) == materialsMap.end())
            {
                materialsMap[material.diffuse_texname] = currentMaterialIndex++;
            }
        }

        std::vector<std::string> remappedMaterials;

        remappedMaterials.resize(materialsMap.size());
        for (auto& [textureName, materialIndex] : materialsMap)
        {
            remappedMaterials[materialIndex] = textureName;
        }

        std::cout << "remapped materials\n";

        std::vector<VertexData> vertexDataVector;
        vertexDataVector.reserve(m_attrib.vertices.size() / 3);

        uint32_t i = 0;
        for (auto& [uniqueVertex, arrayIndex] : vertexes)
        {
            int vertexIndex = uniqueVertex.vertexIndex;
            int normalIndex = uniqueVertex.normalIndex;
            int materialIndex = uniqueVertex.materialIndex;
            int texcoordIndex = uniqueVertex.texcoordIndex;

            VertexData vertexData;

            if (vertexIndex >= 0)
            {
                vertexData.position[0] = vertices[3 * vertexIndex];
                vertexData.position[1] = vertices[3 * vertexIndex + 2];
                vertexData.position[2] = vertices[3 * vertexIndex + 1];
            }
            else
            {
                vertexData.position[0] = 0.0f;
                vertexData.position[1] = 0.0f;
                vertexData.position[2] = 0.0f;
            }

            vertexData.materialId = materialsMap[m_materials[materialIndex].diffuse_texname];

            if (normalIndex >= 0)
            {
                vertexData.normal[0] = normals[3 * normalIndex];
                vertexData.normal[1] = normals[3 * normalIndex + 1];
                vertexData.normal[2] = normals[3 * normalIndex + 2];
            }
            else
            {
                vertexData.normal[0] = 0.0f;
                vertexData.normal[1] = 0.0f;
                vertexData.normal[2] = 0.0f;
            }

            if (texcoordIndex >= 0)
            {
                vertexData.textureCoords[0] = texcoords[2 * texcoordIndex];
                vertexData.textureCoords[1] = texcoords[2 * texcoordIndex + 1];
            }
            else
            {
                vertexData.normal[0] = 0.0f;
                vertexData.normal[1] = 0.0f;
            }

            vertexDataVector.push_back(vertexData);
            arrayIndex = i++;
        }

        materialsMap.clear();
        vertices.clear();
        normals.clear();
        texcoords.clear();
        m_materials.clear();

        std::cout << "remapped vertexes\n";

        std::vector<uint32_t> indexDataVector;
        indexDataVector.reserve(m_attrib.vertices.size() / 3);

        for (auto& shape : m_shapes)
        {
            auto& mesh = shape.mesh;
            auto& indices = mesh.indices;
            for (size_t i = 0; i < indices.size(); i++)
            {
                auto& index = indices[i];
                size_t triangleIndex = i / 3;
                int materialIndex = mesh.material_ids[triangleIndex];
                indexDataVector.push_back(vertexes[UniqueVertexKey{ materialIndex, index.vertex_index, index.normal_index, index.texcoord_index }]);
            }
        }

        m_shapes.clear();

        std::cout << "loaded indexes\n";

        SceneData::SceneHeader header
        {
            checked_narrowing_cast<UINT>(vertexDataVector.size() * sizeof(VertexData)),
            checked_narrowing_cast<UINT>(indexDataVector.size() * sizeof(uint32_t)),
            checked_narrowing_cast<int>(remappedMaterials.size())
        };

        UINT paddedVertexSize = CEIL_TO_POWER_OF_TWO(header.vertexSize, 512);
        UINT vertexPadding = paddedVertexSize - header.vertexSize;
        header.vertexSize = paddedVertexSize;

        DebugFileWriter fileWriter;
        fileWriter.OpenFile(filename.c_str());

        std::cout << "opened " << filename << " for writing\n";

        fileWriter.Write(&header, sizeof(header));

        std::cout << "written header\n";

        struct Texture
        {
            int width, height, bitsPerPixel;
            unsigned char* data;

            Texture()
                : width(0)
                , height(0)
                , bitsPerPixel(0)
                , data(nullptr)
            {}

            ~Texture()
            {
                if (data != nullptr)
                    stbi_image_free(data);
            }
        };

        std::vector<Texture> diffuseTextures;
        diffuseTextures.resize(remappedMaterials.size());

        std::cout << "loading " << remappedMaterials.size() << " textures\n";

        for (size_t i = 0; i < remappedMaterials.size(); ++i)
        {
            auto& material = remappedMaterials[i];
            auto& texture = diffuseTextures[i];

            SceneData::TextureHeader textureHeader{};

            if (material.empty())
            {
                fileWriter.Write(&textureHeader, sizeof(textureHeader));
                continue;
            }

            texture.data = stbi_load(material.c_str(), &texture.width, &texture.height, &texture.bitsPerPixel, 4);

            BLK_ASSERT(texture.data);

            textureHeader.width = texture.width;
            textureHeader.height = texture.height;

            fileWriter.Write(&textureHeader, sizeof(textureHeader));

            std::cout << "texture " << i << " loaded\n";
        }

        materialsMap.clear();

        fileWriter.Write(vertexDataVector.data(), vertexDataVector.size() * sizeof(VertexData));
        vertexDataVector.clear();
        fileWriter.AddPadding(vertexPadding);

        std::cout << "written vertex buffer\n";

        fileWriter.Write(indexDataVector.data(), header.indexSize);
        indexDataVector.clear();

        std::cout << "written index buffer\n";

        static const size_t bytesPerPixel = 4;
        
        for (size_t i = 0; i < diffuseTextures.size(); i++)
        {
            auto& diffuseTexture = diffuseTextures[i];
            size_t width = diffuseTexture.width;
            size_t height = diffuseTexture.height;

            if (width == 0)
            {
                std::cout << "texture " << i << " skipped due to being empty\n";
                continue;
            }

            size_t srcRowPitch = bytesPerPixel * width;
            size_t dstRowPitch = CEIL_TO_POWER_OF_TWO(bytesPerPixel * width, 256);
            size_t textureSize = CEIL_TO_POWER_OF_TWO(dstRowPitch * height, 512);

            unsigned char* textureData = new unsigned char[textureSize];

            unsigned char* dstPtr = textureData;
            unsigned char* srcPtr = diffuseTexture.data;
            for (size_t y = 0; y < height; ++y)
            {
                memcpy(dstPtr, srcPtr, srcRowPitch);
                srcPtr += srcRowPitch;
                dstPtr += dstRowPitch;
            }

            BLK_ASSERT(textureData != nullptr);

            fileWriter.Write(textureData, textureSize);

            for (
                size_t mipWidth = width / 2, mipHeight = height / 2;
                mipWidth > 0 && mipHeight > 0;
                mipWidth /= 2, mipHeight /= 2
                )
            {
                size_t prevMipWidth = mipWidth * 2;
                size_t rowPitch = CEIL_TO_POWER_OF_TWO(bytesPerPixel * mipWidth, 256);
                size_t prevRowPitch = CEIL_TO_POWER_OF_TWO(bytesPerPixel * prevMipWidth, 256);
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
                                memcpy(prevMipRGBA[2 * i + j], &textureData[prevRowPitch * (y * 2 + i) + bytesPerPixel * (x * 2 + j)], bytesPerPixel);
                            }
                        }

                        for (size_t i = 0; i < 4; ++i)
                        {
                            int sum = 0;
                            for (auto &data : prevMipRGBA)
                            {
                                sum += data[i];
                            }
                            rgba[i] = sum / 4;
                        }

                        memcpy(&mipData[rowPitch * y + bytesPerPixel * x], rgba, bytesPerPixel);
                    }
                }

                delete[] textureData;
                textureData = mipData;

                fileWriter.Write(mipData, mipSize);
            }

            delete[] textureData;

            std::cout << "texture " << i << " written\n";
        }

        bool res = fileWriter.Close();
        BLK_ASSERT(res);

        return true;
    }

}
