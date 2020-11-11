#include "stdafx.h"
#include "IndexBufferView.h"
#include "APIWrappers/Resources/Buffers/IndexBuffer.h"

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

    bool IndexBufferView::Initialize(IndexBuffer& indexBuffer, UINT size, DXGI_FORMAT format)
    {
        BLK_ASSERT(m_View.BufferLocation == 0);
        BLK_ASSERT(format == DXGI_FORMAT_R16_UINT || format == DXGI_FORMAT_R32_UINT);

        m_View.BufferLocation = indexBuffer->GetGPUVirtualAddress();
        m_View.SizeInBytes = size;
        m_View.Format = format;

        return true;
    }

    void IndexBufferView::Unload()
    {
        BLK_ASSERT(m_View.BufferLocation != 0);
        m_View = {};
    }

}