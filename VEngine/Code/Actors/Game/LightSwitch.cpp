#include "vpch.h"
#include "LightSwitch.h"
#include "Components/Lights/PointLightComponent.h"

LightSwitch::LightSwitch()
{
    pointLight = CreateComponent(PointLightComponent(), "PointLight");
    rootComponent->AddChild(pointLight);
}

Properties LightSwitch::GetProps()
{
    return __super::GetProps();
}

void LightSwitch::Interact()
{
    pointLight->active = !pointLight->active;
}