#pragma once

#include "RHIStruct.h"
#include <string>
#include "common/Ref.hpp"
#include "common/RefCounted.h"


namespace rhi::impl
{
	class AdapterBase : public RefCounted
	{
	public:
		//api
		virtual DeviceBase* APICreateDevice(const DeviceDesc& desc) = 0;
		void APIGetInfo(AdapterInfo* info) const;
		void APIGetLimits(Limits* limits) const;
		InstanceBase* APIGetInstance() const;
		//internal
		Ref<InstanceBase> GetInstance() const;
	protected:
		AdapterBase(InstanceBase* instance);
		~AdapterBase();

		Ref<InstanceBase> mInstance;

		uint32_t mApiVersion;
		uint32_t mDriverVersion;
		uint32_t mVendorID;
		uint32_t mDeviceID;
		AdapterType mAdapterType;
		std::string mDeviceName;
		Limits mLimits;
	};
}