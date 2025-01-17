#include "vpch.h"
#include "ConditionTrigger.h"
#include "Components/BoxTriggerComponent.h"
#include "Components/Game/ConditionComponent.h"

ConditionTrigger::ConditionTrigger()
{
	condition = CreateComponent<ConditionComponent>("Condition");

	boxTrigger = CreateComponent<BoxTriggerComponent>("BoxTrigger");
	rootComponent = boxTrigger;
}

void ConditionTrigger::Start()
{
	__super::Start();

	boxTrigger->SetTargetAsPlayer();
}

void ConditionTrigger::Tick(float deltaTime)
{
	__super::Tick(deltaTime);

	if (boxTrigger->ContainsTarget())
	{
		condition->CheckCondition();
		SetActive(false);
	}
}

Properties ConditionTrigger::GetProps()
{
	auto props = __super::GetProps();
	props.title = GetTypeName();
	return props;
}
