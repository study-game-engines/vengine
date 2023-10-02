#include "vpch.h"
#include "CrystalLight.h"
#include "Components/Lights/PointLightComponent.h"
#include "Particle/SpriteSheet.h"

CrystalLight::CrystalLight()
{
	pointLight = CreateComponent<PointLightComponent>("PointLight");
	rootComponent = pointLight;

	spriteSheet = CreateComponent<SpriteSheet>("Sprite");
	rootComponent->AddChild(spriteSheet);
}

Properties CrystalLight::GetProps()
{
	auto props = __super::GetProps();
	props.title = GetTypeName();
	return props;
}

void CrystalLight::Create()
{
	__super::Create();

	spriteSheet->SetupSpriteData("Sprites/blue_explosion.png", 5, 5, true);
	spriteSheet->SetLocalScale(0.5f);

	pointLight->SetLightColour(XMFLOAT4(0.9f, 0.9f, 1.f, 1.f));
}
