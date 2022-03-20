#include "stdafx.h"

#include "DStorageQueue.h"

#include "APIWrappers/Resources/Textures/Texture2D.h"
#include "DStorageFactory.h"

namespace Boolka
{

    DStorageQueue::DStorageQueue()
        : m_Queue(nullptr)
    {
    }

    DStorageQueue::~DStorageQueue()
    {
        BLK_ASSERT(m_Queue == nullptr);
    }

    IDStorageQueue* DStorageQueue::Get()
    {
        BLK_ASSERT(m_Queue != nullptr);
        return m_Queue;
    }

    IDStorageQueue* DStorageQueue::operator->()
    {
        return Get();
    }

    bool DStorageQueue::Initialize(Device& device)
    {
        DSTORAGE_QUEUE_DESC desc{};
        desc.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
        desc.Capacity = DSTORAGE_MAX_QUEUE_CAPACITY;
        desc.Priority = DSTORAGE_PRIORITY_HIGH;
        desc.Device = device.Get();

        desc.Name = "DStorageQueue::m_Queue";
        HRESULT hr = device.GetDStorageFactory()->CreateQueue(&desc, IID_PPV_ARGS(&m_Queue));

        BLK_ASSERT(SUCCEEDED(hr));

        if (FAILED(hr))
            return false;

        return m_Fence.Initialize(device);
    }

    void DStorageQueue::Unload()
    {
        m_Fence.Unload();

        BLK_ASSERT(m_Queue != nullptr);
        m_Queue->Release();
        m_Queue = nullptr;
    }

    UINT64 DStorageQueue::SignalCPU()
    {
        return m_Fence.SignalCPU();
    }

    UINT64 DStorageQueue::SignalDStorage()
    {
        return m_Fence.SignalDStorage(*this);
    }

    void DStorageQueue::WaitCPU(UINT64 value)
    {
        m_Fence.WaitCPU(value);
    }

    void DStorageQueue::SyncGPU(CommandQueue& commandQueue)
    {
        UINT64 fenceValue = SignalDStorage();
        SubmitCommands();
        GetFence().WaitGPU(fenceValue, commandQueue);
    }

    void DStorageQueue::Flush()
    {
        UINT64 value = SignalDStorage();
        SubmitCommands();
        WaitCPU(value);
    }

    Fence& DStorageQueue::GetFence()
    {
        return m_Fence;
    }

    void DStorageQueue::EnququeRead(const MemoryBlock& memory, Buffer& buffer,
                                    size_t dstOffset /*= 0*/)
    {
        BLK_ASSERT(m_Queue != nullptr);
        DSTORAGE_REQUEST request{};
        request.Options.SourceType = DSTORAGE_REQUEST_SOURCE_MEMORY;
        request.Options.DestinationType = DSTORAGE_REQUEST_DESTINATION_BUFFER;
        request.Source.Memory.Source = memory.m_Data;
        request.Source.Memory.Size = checked_narrowing_cast<UINT32>(memory.m_Size);
        request.Destination.Buffer.Resource = buffer.Get();
        request.Destination.Buffer.Offset = dstOffset;
        request.Destination.Buffer.Size = checked_narrowing_cast<UINT32>(memory.m_Size);
        request.UncompressedSize = checked_narrowing_cast<UINT32>(memory.m_Size);
        m_Queue->EnqueueRequest(&request);
    }

    void DStorageQueue::EnququeRead(DStorageFile& file, size_t srcOffset, size_t srcSize,
                                    Buffer& buffer, size_t dstOffset)
    {
        BLK_ASSERT(m_Queue != nullptr);
        DSTORAGE_REQUEST request{};
        request.Options.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
        request.Options.DestinationType = DSTORAGE_REQUEST_DESTINATION_BUFFER;
        request.Source.File.Source = file.Get();
        request.Source.File.Offset = srcOffset;
        request.Source.File.Size = checked_narrowing_cast<UINT32>(srcSize);
        request.Destination.Buffer.Resource = buffer.Get();
        request.Destination.Buffer.Offset = dstOffset;
        request.Destination.Buffer.Size = checked_narrowing_cast<UINT32>(srcSize);
        request.UncompressedSize = checked_narrowing_cast<UINT32>(srcSize);
        m_Queue->EnqueueRequest(&request);
    }

    void DStorageQueue::EnququeRead(DStorageFile& file, size_t srcOffset, size_t srcSize,
                                    Texture2D& texture, UINT subresourceIndex, UINT right,
                                    UINT bottom, UINT left /*= 0*/, UINT top /*= 0*/)
    {
        BLK_ASSERT(m_Queue != nullptr);
        DSTORAGE_REQUEST request{};
        request.Options.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
        request.Options.DestinationType = DSTORAGE_REQUEST_DESTINATION_TEXTURE_REGION;
        request.Source.File.Source = file.Get();
        request.Source.File.Offset = srcOffset;
        request.Source.File.Size = checked_narrowing_cast<UINT32>(srcSize);
        request.Destination.Texture.Resource = texture.Get();
        request.Destination.Texture.SubresourceIndex = subresourceIndex;
        request.Destination.Texture.Region = D3D12_BOX{left, top, 0, right, bottom, 1};
        request.UncompressedSize = checked_narrowing_cast<UINT32>(srcSize);
        m_Queue->EnqueueRequest(&request);
    }

    void DStorageQueue::SubmitCommands()
    {
        BLK_ASSERT(m_Queue != nullptr);
        m_Queue->Submit();
    }

} // namespace Boolka