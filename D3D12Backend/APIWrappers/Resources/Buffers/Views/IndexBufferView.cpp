#include "stdafx.h"

#include "IndexBufferView.h"

namespace Boolka
{

    IndexBufferView::IndexBufferView()
        : m_View{}
    {
    }

    IndexBufferView::~IndexBufferView()
    {
        BLK_ASSERT(m_View.BufferLocation == 0);
    }

    bool IndexBufferView::Initialize(Buffer& indexBuffer, UINT size, DXGI_FORMAT format,
                                     UINT64 bufferOffset /*= 0*/)
    {
        BLK_ASSERT(m_View.BufferLocation == 0);
        BLK_ASSERT(format == DXGI_FORMAT_R16_UINT || format == DXGI_FORMAT_R32_UINT);

        m_View.BufferLocation = indexBuffer->GetGPUVirtualAddress() + bufferOffset;
        m_View.SizeInBytes = size;
        m_View.Format = format;

        return true;
    }

    void IndexBufferView::Unload()
    {
        BLK_ASSERT(m_View.BufferLocation != 0);
        m_View = {};
    }

    const D3D12_INDEX_BUFFER_VIEW* IndexBufferView::GetView()
    {
        BLK_ASSERT(m_View.BufferLocation != 0);
        return &m_View;
    }

} // namespace Boolka