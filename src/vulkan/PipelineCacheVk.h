#pragma once

#include <vulkan/vulkan.h>
#include "../PipelineCacheBase.h"
#include "../common/Ref.hpp"

namespace rhi::impl::vulkan
{
    class PipelineCache final : public PipelineCacheBase
    {
    public:
        static Ref<PipelineCache> Create(DeviceBase* device, const PipelineCacheDesc& desc);
        VkPipelineCache GetHandle() const;
        void GetData(void* data, size_t* dataSize) const override;

    private:
        explicit PipelineCache(DeviceBase* device, const PipelineCacheDesc& desc);
        ~PipelineCache() override;
        bool Initialize(const PipelineCacheDesc& desc);
        void DestroyImpl() override;

        VkPipelineCache mHandle = VK_NULL_HANDLE;
    };
} // namespace rhi::impl::vulkan
