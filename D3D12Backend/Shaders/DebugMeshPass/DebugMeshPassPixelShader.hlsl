#include "DebugMeshPassShadersCommon.hlsli"

PSOut main(VSOut In)
{
    PSOut Out = (PSOut)0;
    Out.color = In.color;
    return Out;
}
