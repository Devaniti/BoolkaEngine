#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class Device;

    class ConstantBuffer :
        public Buffer
    {
    public:
        ConstantBuffer() = default;
        ~ConstantBuffer() = default;

        bool Initialize(Device& device, UINT64 size);
        void Unload();
    };

}
