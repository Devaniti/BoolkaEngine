#pragma once

namespace Boolka
{

    class Buffer;
    class CommandQueue;
    class DStorageFile;
    class Texture2D;
    struct MemoryBlock;

    class [[nodiscard]] DStorageQueue
    {
    public:
        DStorageQueue();
        ~DStorageQueue();

        [[nodiscard]] IDStorageQueue* Get();
        [[nodiscard]] IDStorageQueue* operator->();

        bool Initialize(Device& device);
        void Unload();

        [[nodiscard]] UINT64 SignalCPU();
        [[nodiscard]] UINT64 SignalDStorage();
        void WaitCPU(UINT64 value);

        void SyncGPU(CommandQueue& commandQueue);

        void Flush();

        [[nodiscard]] Fence& GetFence();
        void EnququeRead(const MemoryBlock& memory, Buffer& buffer, size_t dstOffset = 0);
        void EnququeRead(DStorageFile& file, size_t srcOffset, size_t srcSize, Buffer& buffer,
                         size_t dstOffset);
        void EnququeRead(DStorageFile& file, size_t srcOffset, size_t srcSize, Texture2D& texture,
                         UINT subresourceIndex, UINT right, UINT bottom, UINT left = 0,
                         UINT top = 0);
        void SubmitCommands();

    private:
        IDStorageQueue* m_Queue;
        Fence m_Fence;
    };

} // namespace Boolka
