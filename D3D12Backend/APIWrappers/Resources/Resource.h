#pragma once

namespace Boolka
{

    class [[nodiscard]] Resource
    {
    public:
        [[nodiscard]] ID3D12Resource* Get();
        [[nodiscard]] ID3D12Resource* operator->();

    protected:
        Resource();
        ~Resource();

        ID3D12Resource* m_Resource;
    };

} // namespace Boolka
