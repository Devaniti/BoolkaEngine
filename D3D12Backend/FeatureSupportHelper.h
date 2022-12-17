#pragma once

namespace Boolka
{

    class Device;

    class FeatureSupportHelper
    {
    public:
        [[nodiscard]] static bool IsSupported(ID3D12Device* device);
    };

} // namespace Boolka
