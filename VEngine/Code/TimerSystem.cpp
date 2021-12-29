#include "TimerSystem.h"

TimerSystem timerSystem;

void TimerSystem::Tick(float deltaTime)
{
	for (int timerIndex = 0; timerIndex < timerItems.size(); timerIndex++)
	{
		timerItems[timerIndex].currentTime += deltaTime;

		if (timerItems[timerIndex].currentTime > timerItems[timerIndex].endTime)
		{
			timerItems[timerIndex].functionToCall();
			timerItems.erase(timerItems.begin() + timerIndex);
		}
	}
}

void TimerSystem::Cleanup()
{
	timerItems.clear();
}

void TimerSystem::SetTimer(float duration, std::function<void()> functionToCall)
{
	TimerItem timerItem = {};
	timerItem.endTime = duration;
	timerItem.currentTime = 0.0f;
	timerItem.functionToCall = functionToCall;

	timerItems.push_back(timerItem);
}
