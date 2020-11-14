#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class Device;

    class IndexBuffer :
        public Buffer
    {
    public:
        IndexBuffer() = default;
        ~IndexBuffer() = default;

        bool Initialize(Device& device, UINT64 size, void* initialData);
        void Unload();
    };

}
