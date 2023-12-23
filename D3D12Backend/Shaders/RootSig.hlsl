#define RootSig \
    "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | " \
               "DENY_HULL_SHADER_ROOT_ACCESS | " \
               "DENY_DOMAIN_SHADER_ROOT_ACCESS | " \
               "DENY_GEOMETRY_SHADER_ROOT_ACCESS), " \
    "CBV(b0), " /* Frame CBV */ \
    "CBV(b1), " /* Pass specific CBVs */ \
    "RootConstants(num32BitConstants=1, b2), " /* CPU constant */ \
    "RootConstants(num32BitConstants=1, b3), " /* GPU indirect constant */ \
    "DescriptorTable(CBV(b0, space=1, numDescriptors = 3, flags = DATA_VOLATILE)," /* CBVs */ \
                    "UAV(u0, numDescriptors = 5, flags = DESCRIPTORS_VOLATILE | DATA_VOLATILE), " /* UAVs */  \
                    "SRV(t0, space=0, numDescriptors = 11, flags = DATA_VOLATILE), " /* Dynamic resources */ \
                    "SRV(t0, space=1, numDescriptors = 8, flags = DESCRIPTORS_VOLATILE | DATA_VOLATILE), " /* Meshlet data */ \
                    "SRV(t0, space=2, numDescriptors = 3, flags = DATA_STATIC), " /* RT data */  \
                    "SRV(t0, space=3, numDescriptors = 1, flags = DATA_STATIC), " /* Sky box */ \
                    "SRV(t0, space=4, numDescriptors = 512, flags = DATA_STATIC)), " /* Scene textures */  \
    "StaticSampler(s0, " \
                  "filter = FILTER_MIN_MAG_MIP_POINT, " \
                  "addressU = TEXTURE_ADDRESS_WRAP, " \
                  "addressV = TEXTURE_ADDRESS_WRAP, " \
                  "addressW = TEXTURE_ADDRESS_WRAP), " \
    "StaticSampler(s1, " \
                  "filter = FILTER_MIN_MAG_LINEAR_MIP_POINT, " \
                  "addressU = TEXTURE_ADDRESS_WRAP, " \
                  "addressV = TEXTURE_ADDRESS_WRAP, " \
                  "addressW = TEXTURE_ADDRESS_WRAP), " \
    "StaticSampler(s2, " \
                  "filter = FILTER_MIN_MAG_MIP_LINEAR, " \
                  "addressU = TEXTURE_ADDRESS_WRAP, " \
                  "addressV = TEXTURE_ADDRESS_WRAP, " \
                  "addressW = TEXTURE_ADDRESS_WRAP), " \
    "StaticSampler(s3, " \
                  "filter = FILTER_ANISOTROPIC, " \
                  "addressU = TEXTURE_ADDRESS_WRAP, " \
                  "addressV = TEXTURE_ADDRESS_WRAP, " \
                  "addressW = TEXTURE_ADDRESS_WRAP, " \
                  "MaxAnisotropy = 16)," \
    "StaticSampler(s4, " \
                  "filter = FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, " \
                  "addressU = TEXTURE_ADDRESS_WRAP, " \
                  "addressV = TEXTURE_ADDRESS_WRAP, " \
                  "addressW = TEXTURE_ADDRESS_WRAP, " \
                  "comparisonFunc = COMPARISON_LESS_EQUAL)"
