#include "AdapterBase.h"
#include "InstanceBase.h"
#include "common/Error.h"

namespace rhi::impl
{
	AdapterBase::AdapterBase(InstanceBase* instance) :
		mInstance(instance)
	{ }

	AdapterBase::~AdapterBase() {}

	void AdapterBase::APIGetInfo(AdapterInfo* info) const
	{
		ASSERT(info != nullptr);
		info->apiVersion = mApiVersion;
		info->driverVersion = mDriverVersion;
		info->deviceID = mDeviceID;
		info->vendorID = mVendorID;
		info->adapterType = mAdapterType;
		info->deviceName = mDeviceName;
	}

	void AdapterBase::APIGetLimits(Limits* limits) const
	{
		*limits = mLimits;
	}

	InstanceBase* AdapterBase::APIGetInstance() const
	{
		Ref<InstanceBase> instance = mInstance;
		return instance.Detach();
	}

	Ref<InstanceBase> AdapterBase::GetInstance() const
	{
		return mInstance;
	}
}