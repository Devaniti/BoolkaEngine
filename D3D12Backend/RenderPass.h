#pragma once

namespace Boolka
{
    class Device;
    class RenderContext;
    class ResourceTracker;

    class RenderPass
    {
    public:
        RenderPass() {};
        virtual ~RenderPass() {};

        virtual bool PrepareRendering() = 0;
        virtual bool Render(RenderContext& renderContext, ResourceTracker& resourceTracker) = 0;

        virtual bool Initialize(Device& device, RenderContext& renderContext, ResourceTracker& resourceTracker) = 0;
        virtual void Unload() = 0;
    };

}

