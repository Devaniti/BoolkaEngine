#include "stdafx.h"

namespace Boolka
{

    static_assert(sizeof(HLSLShared::VertexData1) % 16 == 0,
                  "This struct is used in structured buffer, so for performance reasons its "
                  "size should be multiple of float4");

    static_assert(sizeof(HLSLShared::VertexData2) % 16 == 0,
                  "This struct is used in structured buffer, so for performance reasons its "
                  "size should be multiple of float4");

    static_assert(sizeof(HLSLShared::ObjectData) % 16 == 0,
                  "This struct is used in structured buffer, so for performance reasons its "
                  "size should be multiple of float4");

    static_assert(sizeof(HLSLShared::MeshletData) % 16 == 0,
                  "This struct is used in structured buffer, so for performance reasons its "
                  "size should be multiple of float4");

    namespace SceneData
    {

        bool FormatHeader::IsValid() const
        {
            FormatHeader valid{};
            return (memcmp(signature, valid.signature, sizeof(signature)) == 0) &&
                   (formatVersion == valid.formatVersion);
        }

    } // namespace SceneData
} // namespace Boolka
