#include "CallbackTaskManager.h"

namespace rhi::impl
{
	void CallbackTask::Execute()
	{
		switch (mState)
		{
		case CallbackState::DeviceLoss:
			HandleDeviceLossImpl();
			break;
		case CallbackState::ShutDown:
			HandleShutDownImpl();
			break;
		default:
			FinishImpl();
		}
	}

	void CallbackTask::OnShutDown()
	{
		mState = CallbackState::ShutDown;
	}

	void CallbackTask::OnDeviceLoss()
	{
		mState = CallbackState::DeviceLoss;
	}

	bool CallbackTaskManager::IsEmpty()
	{
		return mStateAndQueue.Use([](auto stateAndQueue) { return stateAndQueue->mTaskQueue.empty(); });
	}

	void CallbackTaskManager::AddCallbackTask(std::unique_ptr<CallbackTask> callbackTask)
	{
		mStateAndQueue.Use([&](auto stateAndQueue) {
			switch (stateAndQueue->mState)
			{
			case CallbackState::ShutDown:
				callbackTask->OnShutDown();
				break;
			case CallbackState::DeviceLoss:
				callbackTask->OnDeviceLoss();
				break;
			default:
				break;
			}
			stateAndQueue->mTaskQueue.push_back(std::move(callbackTask));
			});
	}


	void CallbackTaskManager::HandleDeviceLoss()
	{
		mStateAndQueue.Use([&](auto stateAndQueue) {
			if (stateAndQueue->mState != CallbackState::Normal)
			{
				return;
			}
			stateAndQueue->mState = CallbackState::DeviceLoss;
			for (auto& task : stateAndQueue->mTaskQueue)
			{
				task->OnDeviceLoss();
			}
			});
	}

	void CallbackTaskManager::HandleShutDown()
	{
		mStateAndQueue.Use([&](auto stateAndQueue)
			{
				if (stateAndQueue->mState != CallbackState::Normal)
				{
					return;
				}
				stateAndQueue->mState = CallbackState::ShutDown;
				for (auto& task : stateAndQueue->mTaskQueue)
				{
					task->OnShutDown();
				}
			});
	}

	void CallbackTaskManager::Flush()
	{
		if (IsEmpty())
		{
			return;
		}

		// If a user calls Queue::Submit inside the callback, then the device will be ticked,
		// which in turns ticks the tracker, causing reentrance and dead lock here. To prevent
		// such reentrant call, we remove all the callback tasks from mCallbackTaskManager,
		// update mCallbackTaskManager, then call all the callbacks.
		std::vector<std::unique_ptr<CallbackTask>> allTasks;
		mStateAndQueue.Use([&](auto stateAndQueue) { allTasks.swap(stateAndQueue->mTaskQueue); });

		for (auto& callbackTask : allTasks)
		{
			callbackTask->Execute();
		}
	}
}