#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class VertexBuffer;

    class [[nodiscard]] VertexBufferView
    {
    public:
        VertexBufferView();
        ~VertexBufferView();

        bool Initialize(Buffer& vertexBuffer, UINT size, UINT stride, UINT64 bufferOffset = 0);
        void Unload();

        [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW* GetView();

    private:
        D3D12_VERTEX_BUFFER_VIEW m_View;
    };

} // namespace Boolka
