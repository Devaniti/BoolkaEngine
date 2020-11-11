#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class IndexBuffer;

    class IndexBufferView
    {
    public:
        IndexBufferView();
        ~IndexBufferView();

        bool Initialize(IndexBuffer& indexBuffer, UINT size, DXGI_FORMAT format);
        void Unload();

        const D3D12_INDEX_BUFFER_VIEW* GetView() { BLK_ASSERT(m_View.BufferLocation != 0); return &m_View; };

    private:
        D3D12_INDEX_BUFFER_VIEW m_View;
    };

}
