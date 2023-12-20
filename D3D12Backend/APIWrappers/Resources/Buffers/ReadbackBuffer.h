#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class Device;

    class [[nodiscard]] ReadbackBuffer : public Buffer
    {
    public:
        ReadbackBuffer() = default;
        ~ReadbackBuffer() = default;

        bool Initialize(Device& device, UINT64 size);
        void Unload();

        [[nodiscard]] void* Map(UINT64 readRangeBegin, UINT64 readRangeEnd);
        void Unmap();

        void Readback(void* data, UINT64 size, UINT64 offset = 0);
    };

} // namespace Boolka
