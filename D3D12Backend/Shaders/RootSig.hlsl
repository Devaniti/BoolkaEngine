#define RootSig "RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | " \
                           "DENY_HULL_SHADER_ROOT_ACCESS | " \
                           "DENY_DOMAIN_SHADER_ROOT_ACCESS | " \
                           "DENY_GEOMETRY_SHADER_ROOT_ACCESS), " \
                "CBV(b0), " \
                "CBV(b1), " \
                "SRV(t0), " \
                "SRV(t1), " \
                "SRV(t2), " \
                "DescriptorTable(CBV(b0, space=1, numDescriptors = 4)), " \
                "DescriptorTable(SRV(t0, space=1, numDescriptors = unbounded)), " \
                "DescriptorTable(UAV(u0, space=1, numDescriptors = 4)), " \
                "DescriptorTable(Sampler(s0, space=1, numDescriptors = 8)), " \
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
                              "MaxAnisotropy = 16)"
