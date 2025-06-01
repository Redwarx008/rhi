#include "CommandRecordContextVk.h"

#include <array>

namespace rhi::impl::vulkan
{
    void CommandRecordContext::AddBufferBarrier(const VkBufferMemoryBarrier2& barrier)
    {
        mBufferMemoryBarriers.push_back(barrier);
    }

    void CommandRecordContext::AddTextureBarrier(const VkImageMemoryBarrier2& barrier)
    {
        mImageMemoryBarriers.push_back(barrier);
    }

    void CommandRecordContext::EmitBarriers()
    {
        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = 0;
        dependencyInfo.imageMemoryBarrierCount = static_cast<uint32_t>(mImageMemoryBarriers.size());
        dependencyInfo.pImageMemoryBarriers = mImageMemoryBarriers.data();
        dependencyInfo.bufferMemoryBarrierCount = static_cast<uint32_t>(mBufferMemoryBarriers.size());
        dependencyInfo.pBufferMemoryBarriers = mBufferMemoryBarriers.data();

        vkCmdPipelineBarrier2(commandBufferAndPool.bufferHandle, &dependencyInfo);

        mBufferMemoryBarriers.clear();
        mImageMemoryBarriers.clear();
    }

    void CommandRecordContext::Reset()
    {
        commandBufferAndPool = CommandPoolAndBuffer();
        needsSubmit = false;
        waitSemaphoreSubmitInfos.clear();
        signalSemaphoreSubmitInfos.clear();
        mBufferMemoryBarriers.clear();
        mImageMemoryBarriers.clear();
    }
} // namespace rhi::impl::vulkan
