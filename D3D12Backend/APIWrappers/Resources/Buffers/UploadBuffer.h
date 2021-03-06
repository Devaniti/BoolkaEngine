#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class Device;

    class UploadBuffer : public Buffer
    {
    public:
        UploadBuffer() = default;
        ~UploadBuffer() = default;

        bool Initialize(Device& device, UINT64 size);
        void Unload();

        void* Map();
        void Unmap();

        void Upload(void* data, UINT64 size);
    };

} // namespace Boolka
