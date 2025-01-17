#include "vpch.h"
#include "SavePoint.h"
#include "Components/BoxTriggerComponent.h"
#include "Core/Input.h"
#include "Gameplay/GameUtils.h"
#include "Gameplay/GameInstance.h"

SavePoint::SavePoint()
{
	trigger = BoxTriggerComponent::system.Add("Trigger", this);
	rootComponent = trigger;
}

void SavePoint::Start()
{
	__super::Start();

	trigger->SetTargetAsPlayer();
}

void SavePoint::Tick(float deltaTime)
{
	__super::Tick(deltaTime);

	if (trigger->ContainsTarget())
	{
		if (Input::GetKeyUp(Keys::Enter))
		{
			GameInstance::SetContinueMapName(World::worldFilename);
			GameUtils::SaveGameWorldState();
		}
	}
}

Properties SavePoint::GetProps()
{
	auto props = __super::GetProps();
	props.title = GetTypeName();
	return props;
}
