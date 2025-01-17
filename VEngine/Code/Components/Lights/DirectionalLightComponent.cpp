#include "vpch.h"
#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent()
{
	lightData.intensity = 1.0f;
}

void DirectionalLightComponent::Create()
{
	shadowMapOrthoSize = 30.f;
	layer = CollisionLayers::Editor;
	lightData.lightType = (int)LightType::Directional;
}

Properties DirectionalLightComponent::GetProps()
{
	auto props = __super::GetProps();
	props.title = "DirectionalLight";
	props.Add("Colour", &lightData.colour);
	props.Add("Intensity", &lightData.intensity);
	props.Add("LightEnabled", &lightData.enabled);
	return props;
}

LightData DirectionalLightComponent::GetLightData()
{
	XMStoreFloat4(&lightData.position, GetWorldPositionV());
	XMStoreFloat4(&lightData.direction, GetForwardVectorV());
	return lightData;
}
