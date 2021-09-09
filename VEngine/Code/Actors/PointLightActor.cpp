#include "PointLightActor.h"
#include "Components/Lights/PointLightComponent.h"

PointLightActor::PointLightActor()
{
    pointLight = PointLightComponent::system.Add(this);
    rootComponent = pointLight;
}

Properties PointLightActor::GetProps()
{
    return Actor::GetProps();
}
