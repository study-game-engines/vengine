#include "vpch.h"
#include "GlobalPropSetter.h"
#include "Gameplay/GameInstance.h"
#include "Core/Log.h"

GlobalPropSetter::GlobalPropSetter()
{
	SetEmptyRootComponent();
}

void GlobalPropSetter::Activate()
{
	__super::Activate();

	bool* value = GameInstance::GetGlobalProp<bool>(propertyName);
	*value = propertyValue;
	Log("[%s] set global property [%s] to [%d]", GetName().c_str(), propertyName.c_str(), *value);
}

Properties GlobalPropSetter::GetProps()
{
	auto props = __super::GetProps();
	props.title = GetTypeName();
	props.Add("Property Name", &propertyName);
	props.Add("Property Value", &propertyValue);
	return props;
}