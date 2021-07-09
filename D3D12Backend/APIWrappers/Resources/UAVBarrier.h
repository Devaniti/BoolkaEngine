#pragma once

namespace Boolka
{

    class Resource;
    class CommandList;

    class UAVBarrier
    {
    public:
        static void Barrier(CommandList& commangList, Resource& resource);
        static void BarrierAll(CommandList& commangList);
    };

} // namespace Boolka
