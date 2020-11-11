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
    private:
        bool CheckOptions(Device& device);
        bool CheckShaderModel(Device& device);
    };

}

