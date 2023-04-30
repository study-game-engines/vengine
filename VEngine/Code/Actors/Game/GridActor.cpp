#include "vpch.h"
#include "GridActor.h"
#include "Actors/Game/Unit.h"
#include "Components/MeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Game/DialogueComponent.h"
#include "UI/Game/HealthWidget.h"
#include "Grid.h"
#include "Core/VMath.h"
#include "Physics/Raycast.h"
#include "Gameplay/GameUtils.h"

GridActor::GridActor()
{
	mesh = CreateComponent("Mesh", MeshComponent("cube.vmesh", "test.png"));
	rootComponent = mesh;

	dialogueComponent = CreateComponent<DialogueComponent>("Dialogue");
}

void GridActor::Start()
{
	nextPos = GetPositionV();
	nextRot = GetRotationV();

	SetGridPosition();

	if (!IsActive())
	{
		auto node = GetCurrentNode();
		if (node)
		{
			node->Show();
		}
	}
}

void GridActor::Tick(float deltaTime)
{
	if (!isInPushback)
	{
		SetPosition(VMath::VectorConstantLerp(GetPositionV(), nextPos, deltaTime, 12.5f));
		SetRotation(VMath::QuatConstantLerp(GetRotationV(), nextRot, deltaTime, 12.5f));
	}
	else
	{
		SetPosition(VMath::VectorConstantLerp(GetPositionV(), nextPushbackPosition, deltaTime, 25.f));
	}

	dialogueComponent->SetPosition(GetHomogeneousPositionV());

	if (isInPushback)
	{
		XMVECTOR position = GetPositionV();
		if (!VMath::VecEqual(position, nextPushbackPosition, 0.25f))
		{
			SetPosition(XMVectorLerp(GetPositionV(), nextPushbackPosition, deltaTime * 6.5));

			XMVECTOR rot = GetRotationV();
			SetRotation(XMQuaternionMultiply(rot, XMQuaternionRotationAxis(-GetForwardVectorV(), 5.f * deltaTime)));
		}
		else
		{
			if (hitActorOnPushback)
			{
				hitActorOnPushback->InflictDamage(1);
			}

			isInPushback = false;
			GameUtils::SpawnSpriteSheet("Sprites/explosion.png", GetPositionV(), false, 4, 4);
			GameUtils::PlayAudioOneShot("Audio/armor_light.wav");
			Destroy();
		}
	}
}

Properties GridActor::GetProps()
{
	auto props = __super::GetProps();
	props.title = "GridActor";
	props.Add("Destruct", &isDestructible);
	props.Add("Health", &health);
	props.Add("Interact", &isInteractable);
	props.Add("Inspect", &isInspectable);
	props.Add("Interact Text", &interactText);
	props.Add("InteractKnownText", &interactKnownText);
	props.Add("DisableGridInteract", &disableGridInteract);
	props.Add("Obstacle", &isGridObstacle);
	props.Add("Link Rotation", &canBeRotatedInLink);
	props.Add("Link Movement", &canBeMovedInLink);
	return props;
}

void GridActor::InflictDamage(int damage)
{
	Attacked();

	if (!isDestructible) return;

	health -= damage;
	if (healthWidget)
	{
		healthWidget->healthPoints = health;
	}

	if (health <= 0)
	{
		//Hide node if attacked Unit is the last enemy
		if (dynamic_cast<Unit*>(this))
		{
			GetCurrentNode()->Hide();
		}
		else
		{
			GetCurrentNode()->Show();
		}

		HitResult hit(this);
		GetCurrentNode()->RecalcNodeHeight(hit);
		Destroy();
	}
}

void GridActor::SetGridPosition()
{
	xIndex = std::round(GetPosition().x);
	yIndex = std::round(GetPosition().z);
}

GridNode* GridActor::GetCurrentNode()
{
	if (disableGridInteract)
	{
		return nullptr;
	}

	auto grid = Grid::system.GetFirstActor();
	auto node = grid->GetNode(xIndex, yIndex);
	return node;
}

bool GridActor::CheckNextNodeMoveIsValid()
{
	const int nextXIndex = (int)std::round(nextPos.m128_f32[0]);
	const int nextYIndex = (int)std::round(nextPos.m128_f32[2]);

	auto grid = Grid::system.GetFirstActor();

	if (nextXIndex >= grid->sizeX || nextYIndex >= grid->sizeY
		|| nextXIndex < 0 || nextYIndex < 0)
	{
		nextPos = GetPositionV();
		return false;
	}

	auto nextNodeToMoveTo = grid->GetNode(nextXIndex, nextYIndex);
	if (!nextNodeToMoveTo->active)
	{
		nextPos = GetPositionV();
		return false;
	}

	auto node = grid->GetNode(nextXIndex, nextYIndex);
	nextPos = XMLoadFloat3(&node->worldPosition);

	return true;
}

bool GridActor::Pushback(XMVECTOR direction)
{
	HitResult hit(this);
	if (Raycast(hit, GetPositionV(), direction, 50.f))
	{
		nextPushbackPosition = XMLoadFloat3(&hit.hitPos);
		isInPushback = true;
		hitActorOnPushback = dynamic_cast<GridActor*>(hit.hitActor);

		HitResult nodeRecalcHit(this);
		GetCurrentNode()->RecalcNodeHeight(nodeRecalcHit);

		return true;
	}

	return false;
}

ForwardFace GridActor::GetCurrentForwardFace()
{
	const XMFLOAT3 forward = GetForwardVector();

	const int forwardIndex = static_cast<int>(std::lroundf(forward.z));
	const int rightIndex = static_cast<int>(std::lroundf(forward.x));

	if (forwardIndex > 0) return ForwardFace::positiveZ;
	else if (forwardIndex < 0) return ForwardFace::negativeZ;
	else if (rightIndex > 0) return ForwardFace::positiveX;
	else if (rightIndex < 0) return ForwardFace::negativeX;

	throw new std::exception("something went wrong with forward vector");
}

bool GridActor::HaveMovementAndRotationStopped()
{
	return XMVector4Equal(GetPositionV(), nextPos) && XMQuaternionEqual(GetRotationV(), nextRot);
}

void GridActor::AddNextRotation(XMVECTOR axis, float angle)
{
	nextRot = VMath::AddRotationAngle(nextRot, axis, angle);
}

void GridActor::AddNextPosition(XMVECTOR offset)
{
	nextPos += offset;
}
