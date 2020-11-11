#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class Device;

    class UploadBuffer :
        public Buffer
    {
    public:
        UploadBuffer();
        ~UploadBuffer();

        bool Initialize(Device& device, UINT64 size);
        void Unload();

        void* Map();
        void Unmap();
    };

}
