#pragma once

namespace Boolka
{

    class Resource
    {
    public:
        ID3D12Resource* Get();
        ID3D12Resource* operator->();

    protected:
        Resource();
        ~Resource();

        ID3D12Resource* m_Resource;
    };

} // namespace Boolka
