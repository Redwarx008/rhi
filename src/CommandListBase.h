#pragma once

#include "CommandAllocator.h"
#include "PassResourceUsage.h"
#include "common/RefCounted.h"

namespace rhi::impl
{
    class CommandEncoder;

    class CommandListBase : public RefCounted
    {
    public:
        const CommandListResourceUsage& GetResourceUsages() const;

    protected:
        explicit CommandListBase(DeviceBase* device, CommandEncoder* encoder);
        ~CommandListBase() override;
        DeviceBase* mDevice;
        CommandIterator mCommandIter;
        CommandListResourceUsage mResourceUsages;
    };
} // namespace rhi::impl
