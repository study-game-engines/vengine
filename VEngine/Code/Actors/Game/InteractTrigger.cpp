#include "InteractTrigger.h"
#include "Components/BoxTriggerComponent.h"
#include "GameUtils.h"
#include "UI/Widget.h"
#include "VString.h"
#include "Input.h"

InteractTrigger::InteractTrigger()
{
	trigger = BoxTriggerComponent::system.Add(this);
	rootComponent = trigger;

	widget = new Widget();
}

void InteractTrigger::Start()
{
	trigger->target = (Actor*)GameUtils::GetPlayer();

	widget->pos = GetHomogeneousPositionVector();
	widget->displayText = stows(interactText);
}

void InteractTrigger::Tick(float deltaTime)
{
	if (trigger->ContainsTarget())
	{
		if (Input::GetKeyDown(Keys::Down))
		{
			widget->AddToViewport();
		}
	}
	else
	{
		widget->RemoveFromViewport();
	}
}

Properties InteractTrigger::GetProps()
{
	auto props = Actor::GetProps();
	props.Add("Interact Text", &interactText);
	return props;
}
