#include "VkResourceDeleter.h"
#include "DeviceVk.h"
#include "QueueVk.h"
#include "InstanceVk.h"
#include "AdapterVk.h"
#include "../common/Utils.h"
#include "../common/Error.h"

namespace rhi::impl::vulkan
{
	VkResourceDeleter::VkResourceDeleter(Queue* queue) : mQueue(queue) {}

	VkResourceDeleter::~VkResourceDeleter()
	{
        ASSERT(mBuffersToDelete.Empty());
        ASSERT(mDescriptorPoolsToDelete.Empty());
        ASSERT(mFencesToDelete.Empty());
        ASSERT(mImagesToDelete.Empty());
        ASSERT(mImageViewsToDelete.Empty());
        ASSERT(mPipelinesToDelete.Empty());
        ASSERT(mPipelineLayoutsToDelete.Empty());
        //assert(mQueryPoolsToDelete.Empty());
        ASSERT(mSamplersToDelete.Empty());
        ASSERT(mSemaphoresToDelete.Empty());
        ASSERT(mShaderModulesToDelete.Empty());
        ASSERT(mBufferAllocationToDelete.Empty());
        ASSERT(mImageAllocationToDelete.Empty());
        ASSERT(mSwapchainToDelete.Empty());
	}

    void VkResourceDeleter::Tick(uint64_t completedSerial)
    {
        Device* device = mQueue->GetDevice();
        VkDevice vkDeivce = device->GetHandle();
        VmaAllocator vmaAllocator = device->GetMemoryAllocator();

        mImageAllocationToDelete.ClearUpTo(completedSerial);

        mBufferAllocationToDelete.ClearUpTo(completedSerial);

        for (BufferAllocation bufferAllocation : mBuffersToDelete.IterateUpTo(completedSerial))
        {
            vmaDestroyBuffer(vmaAllocator, bufferAllocation.buffer, bufferAllocation.allocation);
        }
        mBuffersToDelete.ClearUpTo(completedSerial);

        for (ImageAllocation imageAllocation : mImagesToDelete.IterateUpTo(completedSerial))
        {
            vmaDestroyImage(vmaAllocator, imageAllocation.image, imageAllocation.allocation);
        }
        mImagesToDelete.ClearUpTo(completedSerial);

        for (VkPipelineLayout layout : mPipelineLayoutsToDelete.IterateUpTo(completedSerial))
        {
            vkDestroyPipelineLayout(vkDeivce, layout, nullptr);
        }
        mPipelineLayoutsToDelete.ClearUpTo(completedSerial);

        for (VkPipeline pipeline : mPipelinesToDelete.IterateUpTo(completedSerial)) {
            vkDestroyPipeline(vkDeivce, pipeline, nullptr);
        }
        mPipelinesToDelete.ClearUpTo(completedSerial);

        for (VkImageView view : mImageViewsToDelete.IterateUpTo(completedSerial))
        {
            vkDestroyImageView(vkDeivce, view, nullptr);
        }
        mImageViewsToDelete.ClearUpTo(completedSerial);

        for (VkSampler sampler : mSamplersToDelete.IterateUpTo(completedSerial))
        {
            vkDestroySampler(vkDeivce, sampler, nullptr);
        }
        mSamplersToDelete.ClearUpTo(completedSerial);

        for (VkShaderModule module : mShaderModulesToDelete.IterateUpTo(completedSerial))
        {
            vkDestroyShaderModule(vkDeivce, module, nullptr);
        }
        mShaderModulesToDelete.ClearUpTo(completedSerial);

        for (VkDescriptorPool pool : mDescriptorPoolsToDelete.IterateUpTo(completedSerial))
        {
            vkDestroyDescriptorPool(vkDeivce, pool, nullptr);
        }
        mDescriptorPoolsToDelete.ClearUpTo(completedSerial);

        for (VkSemaphore semaphore : mSemaphoresToDelete.IterateUpTo(completedSerial))
        {
            vkDestroySemaphore(vkDeivce, semaphore, nullptr);
        }
        mSemaphoresToDelete.ClearUpTo(completedSerial);

        for (VkFence fence : mFencesToDelete.IterateUpTo(completedSerial))
        {
            vkDestroyFence(vkDeivce, fence, nullptr);
        }
        mFencesToDelete.ClearUpTo(completedSerial);

        for (VkSwapchainKHR swapChain : mSwapchainToDelete.IterateUpTo(completedSerial))
        {
            vkDestroySwapchainKHR(device->GetHandle(), swapChain, nullptr);
        }
        mSwapchainToDelete.ClearUpTo(completedSerial);
    }

    void VkResourceDeleter::DeleteWhenUnused(BufferAllocation bufferAllocation)
    {
        mBuffersToDelete.Push(mQueue->GetPendingSubmitSerial(), bufferAllocation);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkDescriptorPool pool)
    {
        mDescriptorPoolsToDelete.Push(mQueue->GetPendingSubmitSerial(), pool);
    }

    void VkResourceDeleter::DeleteWhenUnused(ImageAllocation imageAllocation)
    {
        mImagesToDelete.Push(mQueue->GetPendingSubmitSerial(), imageAllocation);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkImageView view)
    {
        mImageViewsToDelete.Push(mQueue->GetPendingSubmitSerial(), view);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkPipeline pipeline)
    {
        mPipelinesToDelete.Push(mQueue->GetPendingSubmitSerial(), pipeline);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkPipelineLayout layout)
    {
        mPipelineLayoutsToDelete.Push(mQueue->GetPendingSubmitSerial(), layout);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkSampler sampler)
    {
        mSamplersToDelete.Push(mQueue->GetPendingSubmitSerial(), sampler);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkShaderModule module)
    {
        mShaderModulesToDelete.Push(mQueue->GetPendingSubmitSerial(), module);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkSwapchainKHR swapChain)
    {
        mSwapchainToDelete.Push(mQueue->GetPendingSubmitSerial(), swapChain);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkSemaphore semaphore)
    {
        mSemaphoresToDelete.Push(mQueue->GetPendingSubmitSerial(), semaphore);
    }

    void VkResourceDeleter::DeleteWhenUnused(VkFence fence)
    {
        mFencesToDelete.Push(mQueue->GetPendingSubmitSerial(), fence);
    }

    void VkResourceDeleter::DeleteWhenUnused(Ref<RefCountedHandle<BufferAllocation>> bufferAllocation)
    {
        mBufferAllocationToDelete.Push(mQueue->GetPendingSubmitSerial(), bufferAllocation);
    }

    void VkResourceDeleter::DeleteWhenUnused(Ref<RefCountedHandle<ImageAllocation>> imageAllocatio)
    {
        mImageAllocationToDelete.Push(mQueue->GetPendingSubmitSerial(), imageAllocatio);
    }
}