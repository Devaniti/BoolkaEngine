#pragma once

namespace Boolka
{

    class Resource
    {
    public:
        ID3D12Resource* Get();
        ID3D12Resource* operator->();

#ifdef BLK_RENDER_DEBUG
        void SetDebugName(const wchar_t* format, ...);
#endif

    protected:
        Resource();
        ~Resource();

        ID3D12Resource* m_Resource;
    };

} // namespace Boolka
