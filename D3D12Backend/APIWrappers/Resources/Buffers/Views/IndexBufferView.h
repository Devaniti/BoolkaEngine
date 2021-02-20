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

        bool Initialize(Buffer& indexBuffer, UINT size, DXGI_FORMAT format, UINT64 bufferOffset = 0);
        void Unload();

        const D3D12_INDEX_BUFFER_VIEW* GetView() { BLK_ASSERT(m_View.BufferLocation != 0); return &m_View; };

    private:
        D3D12_INDEX_BUFFER_VIEW m_View;
    };

}
