#pragma once

#include "APIWrappers/InputLayout.h"
#include "APIWrappers/RootSignature.h"
#include "BoolkaCommon/Structures/MemoryBlock.h"

namespace Boolka
{

    template <typename argumentType>
    struct alignas(void*) StateObjectArgumentConvertor;

    template <>
    struct StateObjectArgumentConvertor<D3D12_STATE_OBJECT_FLAGS>
    {
        StateObjectArgumentConvertor() = delete;
        StateObjectArgumentConvertor(D3D12_STATE_SUBOBJECT* currentSubobject,
                                     D3D12_STATE_OBJECT_FLAGS stateObjectFlags)
            : stateObjectConfig{}
        {
            currentSubobject->Type = D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG;
            currentSubobject->pDesc = &stateObjectConfig;

            stateObjectConfig.Flags = stateObjectFlags;
        }

        D3D12_STATE_OBJECT_CONFIG stateObjectConfig;
    };

    struct GlobalRootSignatureParam
    {
        const RootSignature& RootSig;
    };

    template <>
    struct StateObjectArgumentConvertor<GlobalRootSignatureParam>
    {
        StateObjectArgumentConvertor() = delete;
        StateObjectArgumentConvertor(D3D12_STATE_SUBOBJECT* currentSubobject,
                                     const GlobalRootSignatureParam& globalRootSig)
            : GlobalRootSig{}
        {
            currentSubobject->Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
            currentSubobject->pDesc = &GlobalRootSig;

            GlobalRootSig.pGlobalRootSignature = globalRootSig.RootSig.Get();
        }

        D3D12_GLOBAL_ROOT_SIGNATURE GlobalRootSig;
    };

    struct LocalRootSignatureParam
    {
        const RootSignature& RootSig;
    };

    template <>
    struct StateObjectArgumentConvertor<LocalRootSignatureParam>
    {
        StateObjectArgumentConvertor() = delete;
        StateObjectArgumentConvertor(D3D12_STATE_SUBOBJECT* currentSubobject,
                                     const LocalRootSignatureParam& localRootSig)
            : LocalRootSig{}
        {
            currentSubobject->Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
            currentSubobject->pDesc = &LocalRootSig;

            LocalRootSig.pLocalRootSignature = localRootSig.RootSig.Get();
        }

        D3D12_LOCAL_ROOT_SIGNATURE LocalRootSig;
    };

    struct NodeMaskStateObjectParam
    {
        UINT NodeMask;
    };

    template <>
    struct StateObjectArgumentConvertor<NodeMaskStateObjectParam>
    {
        StateObjectArgumentConvertor() = delete;
        StateObjectArgumentConvertor(D3D12_STATE_SUBOBJECT* currentSubobject,
                                     const NodeMaskStateObjectParam& nodeMask)
            : NodeMask{}
        {
            currentSubobject->Type = D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK;
            currentSubobject->pDesc = &NodeMask;

            NodeMask.NodeMask = nodeMask.NodeMask;
        }

        D3D12_NODE_MASK NodeMask;
    };

    template <UINT exportCount>
    struct DXILLibraryParam
    {
        MemoryBlock DXLILib;
        const wchar_t** ExportNames;
    };

    template <UINT exportCount>
    struct StateObjectArgumentConvertor<DXILLibraryParam<exportCount>>
    {
        StateObjectArgumentConvertor() = delete;
        StateObjectArgumentConvertor(D3D12_STATE_SUBOBJECT* currentSubobject,
                                     const DXILLibraryParam<exportCount>& dxilLibrary)
            : LibraryDesc{}
            , Exports{}

        {
            currentSubobject->Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
            currentSubobject->pDesc = &LibraryDesc;

            LibraryDesc.DXILLibrary.pShaderBytecode = dxilLibrary.DXLILib.m_Data;
            LibraryDesc.DXILLibrary.BytecodeLength = dxilLibrary.DXLILib.m_Size;
            LibraryDesc.NumExports = exportCount;
            LibraryDesc.pExports = Exports;

            for (size_t i = 0; i < exportCount; ++i)
            {
                Exports[i].Name = dxilLibrary.ExportNames[i];
                Exports[i].ExportToRename = dxilLibrary.ExportNames[i];
            }
        }

        D3D12_DXIL_LIBRARY_DESC LibraryDesc;
        D3D12_EXPORT_DESC Exports[exportCount];
    };

    struct RaytracingShaderConfigParam
    {
        UINT MaxPayloadSizeInBytes;
        UINT MaxAttributeSizeInBytes;
    };

    template <>
    struct StateObjectArgumentConvertor<RaytracingShaderConfigParam>
    {
        StateObjectArgumentConvertor() = delete;
        StateObjectArgumentConvertor(D3D12_STATE_SUBOBJECT* currentSubobject,
                                     const RaytracingShaderConfigParam& rtShaderConfig)
            : RTShaderConfig{}
        {
            currentSubobject->Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
            currentSubobject->pDesc = &RTShaderConfig;

            RTShaderConfig.MaxPayloadSizeInBytes = rtShaderConfig.MaxPayloadSizeInBytes;
            RTShaderConfig.MaxAttributeSizeInBytes = rtShaderConfig.MaxAttributeSizeInBytes;
        }

        D3D12_RAYTRACING_SHADER_CONFIG RTShaderConfig;
    };

    struct RaytracingPipelineConfigParam
    {
        UINT MaxTraceRecursionDepth;
    };

    template <>
    struct StateObjectArgumentConvertor<RaytracingPipelineConfigParam>
    {
        StateObjectArgumentConvertor() = delete;
        StateObjectArgumentConvertor(D3D12_STATE_SUBOBJECT* currentSubobject,
                                     const RaytracingPipelineConfigParam& rtPipelineConfig)
            : RTPipelineConfig{}
        {
            currentSubobject->Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
            currentSubobject->pDesc = &RTPipelineConfig;

            RTPipelineConfig.MaxTraceRecursionDepth = rtPipelineConfig.MaxTraceRecursionDepth;
        }

        D3D12_RAYTRACING_PIPELINE_CONFIG RTPipelineConfig;
    };

    struct HitGroupParam
    {
        const wchar_t* HitGroupExport;
        const wchar_t* ClosestHitShaderImport;
        const wchar_t* AnyHitShaderImport = nullptr;
    };

    template <>
    struct StateObjectArgumentConvertor<HitGroupParam>
    {
        StateObjectArgumentConvertor() = delete;
        StateObjectArgumentConvertor(D3D12_STATE_SUBOBJECT* currentSubobject,
                                     const HitGroupParam& hitGroup)
            : HitGroupDesc{}
        {
            currentSubobject->Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
            currentSubobject->pDesc = &HitGroupDesc;

            HitGroupDesc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
            HitGroupDesc.HitGroupExport = hitGroup.HitGroupExport;
            HitGroupDesc.AnyHitShaderImport = hitGroup.AnyHitShaderImport;
            HitGroupDesc.ClosestHitShaderImport = hitGroup.ClosestHitShaderImport;
        }

        D3D12_HIT_GROUP_DESC HitGroupDesc;
    };

    // Converts list of wrappers to DX12 readable struct with correct memory layout
    template <typename FirstParam, typename... ArgsType>
    struct alignas(void*) StateObjectStream
    {
        StateObjectStream() = delete;
        StateObjectStream(D3D12_STATE_SUBOBJECT* currentSubobject, const FirstParam& firstParam,
                          const ArgsType&... args)
            : Data(currentSubobject, firstParam, args...){};

        struct Wrapper1
        {
            StateObjectArgumentConvertor<FirstParam> FirstParamValue;
            StateObjectStream<ArgsType...> Tail;

            Wrapper1(D3D12_STATE_SUBOBJECT* currentSubobject, const FirstParam& firstParam,
                     const ArgsType&... args)
                : FirstParamValue(currentSubobject, firstParam)
                , Tail(currentSubobject + 1, args...){};
        };
        struct Wrapper2
        {
            StateObjectArgumentConvertor<FirstParam> FirstParamValue;

            Wrapper2(D3D12_STATE_SUBOBJECT* currentSubobject, const FirstParam& firstParam)
                : FirstParamValue(currentSubobject, firstParam){};
        };

        std::conditional_t<sizeof...(ArgsType) != 0, Wrapper1, Wrapper2> Data;
    };

} // namespace Boolka
