// Copyright 2019 The Dawn & Tint Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "CommandRecordContextVk.h"

#include <array>

namespace rhi::impl::vulkan
{
	void CommandRecordContext::AddBufferBarrier(VkBufferMemoryBarrier2 barrier)
	{
        mBufferMemoryBarriers.push_back(barrier);
	}

    void CommandRecordContext::AddTextureBarrier(VkImageMemoryBarrier2 barrier)
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
}