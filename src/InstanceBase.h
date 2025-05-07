#pragma once

#include "RHIStruct.h"
#include "common/RefCounted.h"

namespace rhi::impl
{
	class AdapterBase;
	class SurfaceBase;
	class InstanceBase : public RefCounted
	{
	public:
		//api
		virtual SurfaceBase* APICreateSurface(void* hwnd, void* hinstance) = 0;
		virtual void APIEnumerateAdapters(AdapterBase** const adapters, uint32_t* adapterCount) = 0;
		// internal
		bool IsDebugLayerEnabled() const;
	protected:
		InstanceBase() = default;
		~InstanceBase() = default;

		bool mDebugLayerEnabled = false;
	};
}