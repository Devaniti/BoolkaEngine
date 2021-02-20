#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class VertexBuffer;

    class VertexBufferView
    {
    public:
        VertexBufferView();
        ~VertexBufferView();

        bool Initialize(Buffer& vertexBuffer, UINT size, UINT stride, UINT64 bufferOffset = 0);
        void Unload();

        const D3D12_VERTEX_BUFFER_VIEW* GetView() { BLK_ASSERT(m_View.BufferLocation != 0); return &m_View; };

    private:
        D3D12_VERTEX_BUFFER_VIEW m_View;
    };

}
