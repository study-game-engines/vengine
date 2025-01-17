#pragma once

#include "SpatialComponent.h"
#include "ComponentSystem.h"
#include <functional>

class Actor;
struct HitResult;

//@Todo: Would like to have an event system for BoxTriggers instead of logic through
//Tick()s constantly calling. Essentially would have it so when a target actor enters and exits,
//and event fires and that's it, like in Unity with OnTriggerEnter() or Unreal.

class BoxTriggerComponent : public SpatialComponent
{
public:
	COMPONENT_SYSTEM(BoxTriggerComponent);

	//default is green
	XMFLOAT4 renderWireframeColour = XMFLOAT4(0.1f, 0.75f, 0.1f, 1.0f);

	BoxTriggerComponent();
	void Tick(float deltaTime) override;
	Properties GetProps() override;

	//Remember to set arg as bounds in world space
	bool Intersects(const BoundingOrientedBox& boundsInWorldSpaceToCheckAgainst);
	bool IntersectsWithAnyBoundingBoxInWorld();

	bool Contains(XMVECTOR point);
	bool ContainsTarget();
	void SetTargetAsPlayer();
	XMVECTOR GetRandomPointInTrigger();

	void SetExtents(float x, float y, float z);
	XMFLOAT3 GetExtents() const;

	bool QuickInPlaceBoxCast(HitResult& hitResult, bool drawDebug);

	Actor* GetTargetActor() { return targetActor; }
	Actor* SetTargetActor(Actor* actor) { targetActor = actor; }

	void SetTriggerEnterCallback(std::function<void()> callback);
	void SetTriggerExitCallback(std::function<void()> callback);

private:
	//On trigger enter/exist callback logic for target actor intersection
	void TargetActorIntersectCallbackLogic();

	std::function<void()> triggerEnterCallback;
	std::function<void()> triggerExitCallback;

	Actor* targetActor = nullptr;

	bool targetActorIntersecting = false;
};
