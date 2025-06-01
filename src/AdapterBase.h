#pragma once

#include <string>
#include "RHIStruct.h"
#include "common/Ref.hpp"
#include "common/RefCounted.h"


namespace rhi::impl
{
    class AdapterBase : public RefCounted
    {
    public:
        // api
        virtual DeviceBase* APICreateDevice(const DeviceDesc& desc) = 0;
        void APIGetInfo(AdapterInfo* info) const;
        void APIGetLimits(Limits* limits) const;
        InstanceBase* APIGetInstance() const;
        // internal
        Ref<InstanceBase> GetInstance() const;

    protected:
        AdapterBase(InstanceBase* instance);
        ~AdapterBase() override;

        Ref<InstanceBase> mInstance;

        uint32_t mApiVersion = 0;
        uint32_t mDriverVersion = 0;
        uint32_t mVendorID = 0;
        uint32_t mDeviceID = 0;
        AdapterType mAdapterType = AdapterType::Unknown;
        std::string mDeviceName;
        Limits mLimits = {};
    };
} // namespace rhi::impl
