#pragma once

namespace Boolka
{

    class Device;

    class FeatureSupportHelper
    {
    public:
        FeatureSupportHelper();
        ~FeatureSupportHelper();

        bool Initialize(Device& device);
        void Unload();
    };

} // namespace Boolka
