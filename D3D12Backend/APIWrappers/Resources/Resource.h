#pragma once

namespace Boolka
{

    class Resource
    {
    public:
        ID3D12Resource* Get() { BLK_ASSERT(m_Resource != nullptr); return m_Resource; }
        ID3D12Resource* operator->() { return Get(); }

    protected:
        Resource();
        ~Resource();

        ID3D12Resource* m_Resource;
    };

}
