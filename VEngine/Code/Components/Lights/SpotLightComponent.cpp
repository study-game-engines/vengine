#include "vpch.h"
#include "SpotLightComponent.h"

void SpotLightComponent::Create()
{
    layer = CollisionLayers::Editor;
    lightData.lightType = (int)LightType::Spot;
}

Properties SpotLightComponent::GetProps()
{
    auto props = __super::GetProps();
    props.title = "SpotLight";
    props.Add("Colour", &lightData.colour);
    props.Add("Spot Angle", &lightData.spotAngle);
    props.Add("Intensity", &lightData.intensity);
    props.Add("LightEnabled", &lightData.enabled);
    return props;
}

Light SpotLightComponent::GetLightData()
{
    XMStoreFloat4(&lightData.position, GetWorldPositionV());
    return lightData;
}
