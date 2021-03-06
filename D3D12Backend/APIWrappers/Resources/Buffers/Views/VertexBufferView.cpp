#include "stdafx.h"

#include "VertexBufferView.h"

namespace Boolka
{

    VertexBufferView::VertexBufferView()
        : m_View{}
    {
    }

    VertexBufferView::~VertexBufferView()
    {
        BLK_ASSERT(m_View.BufferLocation == 0);
    }

    bool VertexBufferView::Initialize(Buffer& vertexBuffer, UINT size, UINT stride,
                                      UINT64 bufferOffset /*= 0*/)
    {
        BLK_ASSERT(m_View.BufferLocation == 0);

        m_View.BufferLocation = vertexBuffer->GetGPUVirtualAddress() + bufferOffset;
        m_View.SizeInBytes = size;
        m_View.StrideInBytes = stride;

        return true;
    }

    void VertexBufferView::Unload()
    {
        BLK_ASSERT(m_View.BufferLocation != 0);
        m_View = {};
    }

    const D3D12_VERTEX_BUFFER_VIEW* VertexBufferView::GetView()
    {
        BLK_ASSERT(m_View.BufferLocation != 0);
        return &m_View;
    }

} // namespace Boolka