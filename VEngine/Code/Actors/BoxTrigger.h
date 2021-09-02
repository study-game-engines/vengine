#pragma once
#include "Actor.h"
#include "ActorSystem.h"

struct BoxTrigger : Actor
{
	ACTOR_SYSTEM(BoxTrigger)

	struct MeshComponent* boxMesh;

	BoxTrigger();
	virtual Properties GetProps() override;
};
