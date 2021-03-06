#pragma once

namespace Boolka
{

    class InputLayout
    {
    public:
        InputLayout();
        ~InputLayout();

        void Initialize(UINT numEntries);
        void Unload();

        void SetEntry(size_t index, const D3D12_INPUT_ELEMENT_DESC& desc);

        struct InputLayoutHeader
        {
            UINT m_NumEntries;
        };

        BLK_IS_PLAIN_DATA_ASSERT(InputLayoutHeader);

        struct InputLayoutEntry
        {
            char SemanticName[BLK_D3D12_SEMANTIC_MAX_LENGTH];
        };

        BLK_IS_PLAIN_DATA_ASSERT(InputLayoutEntry);
        BLK_IS_PLAIN_DATA_ASSERT(D3D12_INPUT_ELEMENT_DESC);

        void FillInputLayoutDesc(D3D12_INPUT_LAYOUT_DESC& desc);

    private:
        InputLayoutHeader m_Header;
        InputLayoutEntry* m_Entries;
        D3D12_INPUT_ELEMENT_DESC* m_NativeEntries;
    };

} // namespace Boolka
