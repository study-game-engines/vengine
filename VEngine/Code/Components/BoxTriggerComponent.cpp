#include "vpch.h"
#include "BoxTriggerComponent.h"
#include "Components/MeshComponent.h"
#include "Core/VMath.h"
#include "Actors/Game/Player.h"
#include "Physics/Raycast.h"

BoxTriggerComponent::BoxTriggerComponent()
{
	layer = CollisionLayers::Editor;

	//Keep in mind with triggers that you want them just slightly smaller than 1x1x1 on the grid
	//so you're not always getting Intersects results from the bounding volumes.
	boundingBox.Extents = XMFLOAT3(0.45f, 0.45f, 0.45f);
}

void BoxTriggerComponent::Tick(float deltaTime)
{
	TargetActorIntersectCallbackLogic();
}

Properties BoxTriggerComponent::GetProps()
{
	auto props = __super::GetProps();
	props.title = GetTypeName();
	props.Add("Extents", &boundingBox.Extents);
	props.Add("Offset", &boundingBox.Center);
	return props;
}

bool BoxTriggerComponent::Intersects(const BoundingOrientedBox& boundsInWorldSpaceToCheckAgainst)
{
	const BoundingOrientedBox bbInWorld = GetBoundsInWorldSpace();
	return bbInWorld.Intersects(boundsInWorldSpaceToCheckAgainst);
}

bool BoxTriggerComponent::Contains(XMVECTOR point)
{
	const BoundingOrientedBox bb = GetBoundsInWorldSpace();
	return bb.Contains(point);
}

bool BoxTriggerComponent::ContainsTarget()
{
	if (targetActor)
	{
		XMVECTOR targetPos = targetActor->GetPositionV();
		bool result = Contains(targetPos);
		return result;
	}

	return false;
}

void BoxTriggerComponent::SetTargetAsPlayer()
{
	targetActor = (Actor*)Player::system.GetFirstActor();
}

XMVECTOR BoxTriggerComponent::GetRandomPointInTrigger()
{
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, GetWorldPositionV());

	float lowX = pos.x - boundingBox.Extents.x;
	float highX = pos.x + boundingBox.Extents.x;

	float lowY = pos.y - boundingBox.Extents.y;
	float highY = pos.y + boundingBox.Extents.y;

	float lowZ = pos.z - boundingBox.Extents.z;
	float highZ = pos.z + boundingBox.Extents.z;

	XMFLOAT3 result{};
	result.x = VMath::RandomRange(lowX, highX);
	result.y = VMath::RandomRange(lowY, highY);
	result.z = VMath::RandomRange(lowZ, highZ);

	return XMLoadFloat3(&pos);
}

bool BoxTriggerComponent::IntersectsWithAnyBoundingBoxInWorld()
{
	for (auto& mesh : MeshComponent::system.GetComponents())
	{
		const auto thisTriggerBounds = GetBoundsInWorldSpace();
		if (thisTriggerBounds.Intersects(mesh->GetBoundsInWorldSpace()))
		{
			return true;
		}
	}

	return false;
}

void BoxTriggerComponent::SetExtents(float x, float y, float z)
{
	boundingBox.Extents = XMFLOAT3(x, y, z);
}

XMFLOAT3 BoxTriggerComponent::GetExtents() const
{
	return boundingBox.Extents;
}

bool BoxTriggerComponent::QuickInPlaceBoxCast(HitResult& hitResult, bool drawDebug)
{
	return SimpleBoxCast(GetWorldPositionV(), boundingBox.Extents, hitResult, drawDebug, true);
}

void BoxTriggerComponent::SetTriggerEnterCallback(std::function<void()> callback)
{
	triggerEnterCallback = callback;
}

void BoxTriggerComponent::SetTriggerExitCallback(std::function<void()> callback)
{
	triggerExitCallback = callback;
}

void BoxTriggerComponent::TargetActorIntersectCallbackLogic()
{
	if (targetActor)
	{
		const bool newIntersectingValue = ContainsTarget();
		if (targetActorIntersecting == !newIntersectingValue)
		{
			targetActorIntersecting = newIntersectingValue;

			switch (targetActorIntersecting)
			{
			case true:
				if (triggerEnterCallback) //Note that this is std::function's operator.
				{
					triggerEnterCallback();
				}
				break;
			case false:
				if (triggerExitCallback)
				{
					triggerExitCallback();
				}
				break;
			}
		}
	}
}
