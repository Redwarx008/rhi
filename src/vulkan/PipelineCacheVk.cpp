#include "../common/Utils.h"
#include "PipelineCacheVk.h"
#include "DeviceVk.h"
#include "ErrorsVk.h"
#include "VulkanUtils.h"



namespace rhi::impl::vulkan
{
    Ref<PipelineCache> PipelineCache::Create(DeviceBase* device, const PipelineCacheDesc& desc)
    {
        Ref<PipelineCache> pipelineCache = AcquireRef(new PipelineCache(device, desc));
        if (!pipelineCache->Initialize(desc))
        {
            return nullptr;
        }
        pipelineCache->TrackResource();
        return pipelineCache;
    }

    PipelineCache::PipelineCache(DeviceBase* device, const PipelineCacheDesc& desc)
        : PipelineCacheBase(device, desc)
    {}

    PipelineCache::~PipelineCache() = default;

    void PipelineCache::DestroyImpl()
    {
        if (mHandle)
        {
            const Device* device = checked_cast<Device>(mDevice);
            vkDestroyPipelineCache(device->GetHandle(), mHandle, nullptr);
            mHandle = VK_NULL_HANDLE;
        }
    }

    bool PipelineCache::Initialize(const PipelineCacheDesc& desc)
    {
        Device* device = checked_cast<Device>(mDevice);

        VkPipelineCacheCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

        if (desc.data != nullptr && desc.dataSize > sizeof(VkPipelineCacheHeaderVersionOne))
        {
            const auto& vkProps = device->GetVkDeviceInfo().properties;

            VkPipelineCacheHeaderVersionOne HeaderVersion;
            std::memcpy(&HeaderVersion, desc.data, sizeof(HeaderVersion));

            if (HeaderVersion.headerVersion == VK_PIPELINE_CACHE_HEADER_VERSION_ONE &&
                HeaderVersion.headerSize == 32 && // from specs
                HeaderVersion.deviceID == vkProps.deviceID && HeaderVersion.vendorID == vkProps.vendorID &&
                std::memcmp(
                        HeaderVersion.pipelineCacheUUID, vkProps.pipelineCacheUUID, sizeof(HeaderVersion.pipelineCacheUUID)) == 0)
            {
                createInfo.pInitialData = desc.data;
                createInfo.initialDataSize = desc.dataSize;
            }
        }

        VkResult err = vkCreatePipelineCache(device->GetHandle(), &createInfo, nullptr, &mHandle);
        CHECK_VK_RESULT_FALSE(err, "CreatePipelineCache");

        SetDebugName(device, mHandle, "PipelineCache", GetName());

        return true;
    }

    VkPipelineCache PipelineCache::GetHandle() const
    {
        return mHandle;
    }

    void PipelineCache::GetData(void* data, size_t* dataSize) const
    {
        ASSERT(mHandle);
        const Device* device = checked_cast<Device>(mDevice);

        VkResult err = vkGetPipelineCacheData(device->GetHandle(), mHandle, dataSize, data);
        CHECK_VK_RESULT(err, "GetPipelineCacheData");
    }
} // namespace rhi::impl::vulkan
