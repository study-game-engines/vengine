#pragma once
#include "Actor.h"
#include "ActorSystem.h"

struct Billboard : Actor
{
	ACTOR_SYSTEM(Billboard)

	struct MeshComponent* mesh = nullptr;

	Billboard();
	virtual void Tick(float deltaTime) override;
	virtual Properties GetProps() override;
};
