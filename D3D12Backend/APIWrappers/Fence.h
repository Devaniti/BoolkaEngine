#pragma once

namespace Boolka
{

    class Device;
    class CommandQueue;
    class DStorageQueue;

    class [[nodiscard]] Fence
    {
    public:
        Fence();
        ~Fence();

        bool Initialize(Device& device);
        void Unload();

        [[nodiscard]] ID3D12Fence1* Get();
        [[nodiscard]] ID3D12Fence1* operator->();

        [[nodiscard]] UINT64 SignalCPU();
        [[nodiscard]] UINT64 SignalGPU(CommandQueue& commandQueue);
        [[nodiscard]] UINT64 SignalDStorage(DStorageQueue& dstorageQueue);

        void SignalCPUWithValue(UINT64 value);
        void SignalGPUWithValue(UINT64 value, CommandQueue& commandQueue);
        void SignalDStorageWithValue(UINT64 value, DStorageQueue& dstorageQueue);

        void WaitCPU(UINT64 value);
        void WaitGPU(UINT64 value, CommandQueue& commandQueue);

        static void WaitCPUMultiple(size_t count, Fence** fences, UINT64* values);

    private:
        ID3D12Fence1* m_Fence;
        UINT64 m_ExpactedValue;
        HANDLE m_CPUEvent;
    };

} // namespace Boolka
