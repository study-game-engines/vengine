#pragma once
#include "Unit.h"

struct DialogueWidget;

//Actor that can be talked to and interactied with.
struct NPC : Unit
{
	ACTOR_SYSTEM(NPC);

	bool isQuickDialogueActive = false;

	//Quick dialogue to show when NPC spawns. If empty, nothing shows.
	std::wstring spawnText;
	DialogueWidget* spawnTextWidget = nullptr;

	//Have to use a Tick() timer with NPCs for quicktalk because of null destroyed npcs
	float quickTalkTimer = 0.f;

	NPC();
	void Create() override;
	void Start() override;
	void Tick(float deltaTime) override;
	Properties GetProps() override;

	//A once off dialogue popup used like interact popup is.
	void QuickTalkTo();
	void EndQuickTalkTo();

	void TryToReachTargetNode();

	int nodeToMoveToXIndex = 0;
	int nodeToMoveToYIndex = 0;

	bool tryToReachTargetNode = false;
	bool canReachTargetNode = false;
};
