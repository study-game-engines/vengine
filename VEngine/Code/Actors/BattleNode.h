#pragma once
#include "Actor.h"
#include "ActorSystem.h"

struct InstanceMeshComponent;

struct BattleNode : Actor
{
	ACTOR_SYSTEM(BattleNode)

	InstanceMeshComponent* nodeMesh = nullptr;

	int sizeX;
	int sizeY;

	BattleNode();
	virtual void Start() override;
	virtual Properties GetProps() override;
};
