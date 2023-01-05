#pragma once

#include "Actor.h"
#include "ActorSystem.h"

struct Polyboard;

struct PolyboardActor : Actor
{
	ACTOR_SYSTEM(PolyboardActor);

	Polyboard* polyboard = nullptr;

	PolyboardActor();
	virtual Properties GetProps() override;
};
