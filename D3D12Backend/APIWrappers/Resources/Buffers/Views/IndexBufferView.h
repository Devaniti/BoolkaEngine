#pragma once
#include "APIWrappers/Resources/Buffers/Buffer.h"

namespace Boolka
{

    class IndexBuffer;

    class [[nodiscard]] IndexBufferView
    {
    public:
        IndexBufferView();
        ~IndexBufferView();

        bool Initialize(Buffer& indexBuffer, UINT size, DXGI_FORMAT format,
                        UINT64 bufferOffset = 0);
        void Unload();

        [[nodiscard]] const D3D12_INDEX_BUFFER_VIEW* GetView();

    private:
        D3D12_INDEX_BUFFER_VIEW m_View;
    };

} // namespace Boolka
