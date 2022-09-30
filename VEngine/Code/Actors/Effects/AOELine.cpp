#include "vpch.h"
#include "AOELine.h"
#include "Components/MeshComponent.h"
#include "Render/Material.h"

AOELine::AOELine()
{
	mesh = CreateComponent(MeshComponent("node.fbx", "Particle/orange_gradient.png"), "Mesh");
	rootComponent = mesh;

	mesh->SetBlendState(BlendStates::Default);
	mesh->GetMaterial()->materialShaderData.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
}

Properties AOELine::GetProps()
{
	return __super::GetProps();
}