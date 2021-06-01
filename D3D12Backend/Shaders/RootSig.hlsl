#define RootSig \
    "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | " \
               "DENY_HULL_SHADER_ROOT_ACCESS | " \
               "DENY_DOMAIN_SHADER_ROOT_ACCESS | " \
               "DENY_GEOMETRY_SHADER_ROOT_ACCESS), " \
    "CBV(b0), " \
    "CBV(b1), " \
    "RootConstants(num32BitConstants=2, b2), " \
    "DescriptorTable(UAV(u0), " \
                    "SRV(t0, space=0, numDescriptors = 10, flags = DATA_VOLATILE)), " \
    "DescriptorTable(SRV(t0, space=1, numDescriptors = unbounded, flags = DATA_STATIC)), " \
    "DescriptorTable(SRV(t0, space=2, numDescriptors = 11, flags = DATA_STATIC)), " \
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
