#pragma once

#include "common/MutexProtected.hpp"
#include <memory>

namespace rhi::impl
{
	enum class CallbackState 
	{
		Normal,
		ShutDown,
		DeviceLoss,
	};

	class CallbackTask
	{
	public:
		virtual ~CallbackTask() = default;
		void Execute();
		void OnShutDown();
		void OnDeviceLoss();
	protected:
		virtual void FinishImpl() = 0;
		virtual void HandleShutDownImpl() = 0;
		virtual void HandleDeviceLossImpl() = 0;
	private:
		CallbackState mState = CallbackState::Normal;
	};

	class CallbackTaskManager
	{
	public :
		void AddCallbackTask(std::unique_ptr<CallbackTask> callbackTask);
		bool IsEmpty();
		void HandleDeviceLoss();
		void HandleShutDown();
		void Flush();
	private:
		struct StateAndQueue 
		{
			CallbackState mState = CallbackState::Normal;
			std::vector<std::unique_ptr<CallbackTask>> mTaskQueue;
		};
		MutexProtected<StateAndQueue> mStateAndQueue;
	};
}