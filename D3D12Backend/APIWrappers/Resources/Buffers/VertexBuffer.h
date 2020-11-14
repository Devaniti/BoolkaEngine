#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class Device;

    class VertexBuffer :
        public Buffer
    {
    public:
        VertexBuffer() = default;
        ~VertexBuffer() = default;

        bool Initialize(Device& device, UINT64 size, void* initialData);
        void Unload();
    };

}
