#include "stdafx.h"

#include "PipelineStateLibrary.h"

#ifdef BLK_ENABLE_PIPELINE_LIBRARY

#include "BoolkaCommon/Algorithms/Hashing.h"
#include "BoolkaCommon/DebugHelpers/DebugFileWriter.h"
#include "Containers/RenderCacheContainer.h"
#include "PipelineState.h"

namespace Boolka
{

    PipelineStateLibrary::PipelineStateLibrary()
        : m_PipelineStateLibrary(nullptr)
    {
    }

    PipelineStateLibrary::~PipelineStateLibrary()
    {
        BLK_ASSERT(m_PipelineStateLibrary == nullptr);
    }

    bool PipelineStateLibrary::Initialize(Device& device, RenderCacheContainer& renderCache)
    {
        BLK_ASSERT(m_PipelineStateLibrary == nullptr);

        if (!renderCache.PSOCacheReader.IsOpen())
        {
            g_WDebugOutput << "No cached pipeline library provided. Creating new library."
                           << std::endl;
            return Initialize(device);
        }

        renderCache.PSOCacheReader.WaitData();

        D3D12_MESSAGE_ID filterList[] = {
            D3D12_MESSAGE_ID_CREATEPIPELINELIBRARY_DRIVERVERSIONMISMATCH,
            D3D12_MESSAGE_ID_CREATEPIPELINELIBRARY_ADAPTERVERSIONMISMATCH};
        BLK_RENDER_DEBUG_ONLY(device.FilterMessage(filterList, ARRAYSIZE(filterList)));
        HRESULT hr =
            device->CreatePipelineLibrary(renderCache.PSOCache.m_Data, renderCache.PSOCache.m_Size,
                                          IID_PPV_ARGS(&m_PipelineStateLibrary));
        BLK_RENDER_DEBUG_ONLY(device.RemoveLastMessageFilter());
        BLK_ASSERT(hr == D3D12_ERROR_DRIVER_VERSION_MISMATCH ||
                   hr == D3D12_ERROR_ADAPTER_NOT_FOUND || SUCCEEDED(hr));

#ifdef BLK_DEBUG
        if (!FAILED(hr))
        {
            g_WDebugOutput << "Loaded serialized pipeline library. CRC32:0x" << std::hex
                           << Hashing::CRC32(renderCache.PSOCache) << std::dec << std::endl;
        }
#endif

        renderCache.PSOCacheReader.CloseFile();
        renderCache.PSOCacheReader.FreeData(renderCache.PSOCache);

        if (FAILED(hr))
        {
            g_WDebugOutput << "Failed to load cached pipeline library with error: 0x" << std::hex
                           << hr << std::dec << ". Creating new pipeline library." << std::endl;
            return Initialize(device);
        }

        return true;
    }

    bool PipelineStateLibrary::Initialize(Device& device)
    {
        BLK_ASSERT(m_PipelineStateLibrary == nullptr);
        HRESULT hr =
            device->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(&m_PipelineStateLibrary));
        BLK_ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
            return false;

        return true;
    }

    bool PipelineStateLibrary::Initialize(Device& device, const MemoryBlock& CachedPSOLibrary)
    {
        BLK_ASSERT(m_PipelineStateLibrary == nullptr);

        HRESULT hr = device->CreatePipelineLibrary(CachedPSOLibrary.m_Data, CachedPSOLibrary.m_Size,
                                                   IID_PPV_ARGS(&m_PipelineStateLibrary));
        BLK_ASSERT(hr == D3D12_ERROR_DRIVER_VERSION_MISMATCH ||
                   hr == D3D12_ERROR_ADAPTER_NOT_FOUND || SUCCEEDED(hr));

        return SUCCEEDED(hr);
    }

    ID3D12PipelineLibrary1* PipelineStateLibrary::Get()
    {
        BLK_ASSERT(m_PipelineStateLibrary != nullptr);
        return m_PipelineStateLibrary;
    }

    ID3D12PipelineLibrary1* PipelineStateLibrary::operator->()
    {
        return Get();
    }

    void PipelineStateLibrary::Store(Device& device, const wchar_t* name, PipelineState& PSO)
    {
        BLK_ASSERT(m_PipelineStateLibrary != nullptr);
        HRESULT hr = m_PipelineStateLibrary->StorePipeline(name, PSO.Get());
        BLK_ASSERT(SUCCEEDED(hr) || hr == E_INVALIDARG);
        // If we already have that name in cache, but with different PSO
        if (hr == E_INVALIDARG)
        {
            g_WDebugOutput << "Found stale PSO in PSO Library:\"" << name
                           << "\". Resetting PSO library." << std::endl;
            Reset(device);
            hr = m_PipelineStateLibrary->StorePipeline(name, PSO.Get());
            BLK_ASSERT(SUCCEEDED(hr));
        }
    }

    void PipelineStateLibrary::SaveToDisk(const wchar_t* filename)
    {
        MemoryBlock PSOLibraryCache{};
        if (Serialize(PSOLibraryCache))
        {
            return;
        }
        if (!DebugFileWriter::WriteFile(filename, PSOLibraryCache))
        {
            g_WDebugOutput << "Failed to write pipeline library to disk." << std::endl;
        }
        FreeData(PSOLibraryCache);
    }

    bool PipelineStateLibrary::Serialize(MemoryBlock& data)
    {
        BLK_ASSERT(m_PipelineStateLibrary != nullptr);

        data.m_Size = m_PipelineStateLibrary->GetSerializedSize();
        data.m_Data = VirtualAlloc(NULL, data.m_Size, MEM_COMMIT, PAGE_READWRITE);

        HRESULT hr = m_PipelineStateLibrary->Serialize(data.m_Data, data.m_Size);
        BLK_ASSERT(SUCCEEDED(hr));
        if (FAILED(hr))
        {
            FreeData(data);
            return false;
        }

#ifdef BLK_DEBUG
        g_WDebugOutput << "Serialized pipeline library. CRC32:0x" << std::hex
                       << Hashing::CRC32(data) << std::dec << std::endl;
#endif

        return true;
    }

    void PipelineStateLibrary::FreeData(MemoryBlock& data)
    {
        BLK_ASSERT(data.m_Size != 0);
        BLK_ASSERT(data.m_Data != nullptr);
        VirtualFree(data.m_Data, 0, MEM_RELEASE);
        data = {};
    }

    void PipelineStateLibrary::Unload()
    {
        BLK_ASSERT(m_PipelineStateLibrary != nullptr);

        m_PipelineStateLibrary->Release();
        m_PipelineStateLibrary = nullptr;
    }

    bool PipelineStateLibrary::Reset(Device& device)
    {
        BLK_ASSERT(m_PipelineStateLibrary != nullptr);

        m_PipelineStateLibrary->Release();
        m_PipelineStateLibrary = nullptr;
        return Initialize(device);
    }

} // namespace Boolka

#endif
