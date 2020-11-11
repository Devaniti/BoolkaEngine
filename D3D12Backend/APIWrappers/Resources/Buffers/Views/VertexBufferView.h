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

        bool Initialize(VertexBuffer& vertexBuffer, UINT size, UINT stride);
        void Unload();

        const D3D12_VERTEX_BUFFER_VIEW* GetView() { BLK_ASSERT(m_View.BufferLocation != 0); return &m_View; };

    private:
        D3D12_VERTEX_BUFFER_VIEW m_View;
    };

}
