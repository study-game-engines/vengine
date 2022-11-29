#pragma once

#include "GridActor.h"
#include <memory>
#include "VEnum.h"
#include "Gameplay/BattleEnums.h"
#include "Gameplay/UnitSkills/UnitSkill.h"

struct GridNode;
struct MemoryComponent;
struct EntranceTrigger;
struct Polyboard;
class UnitSkill;
class SkillNode;
struct Memory;

//Units are battle ready actors and really only move and fight.
struct Unit : GridActor
{
	ACTOR_SYSTEM(Unit);

	//Battle states
	struct BattleStates
	{
		inline static std::string fight = "fight"; //Move towards player/destination
		inline static std::string evade = "evade"; //Move away from destination
		inline static std::string wander = "wander"; //Move around randomly
		inline static std::string escape = "escape"; //Esacpe to nearest EntranceTrigger
	};

	VEnum battleState;

	MemoryComponent* memoryOnDeath = nullptr;

	//Meant to show a unit's current focus in battle and in world
	Polyboard* intentBeam = nullptr;

	EntranceTrigger* entranceToEscapeTo = nullptr;

	//Text to display on unit's death during battle
	std::wstring deathText;

	float moveSpeed = 3.0f;

	int movementPoints = 1;
	int movementPathNodeIndex = 0;

	int numOfAttacks = 1;

	int attackPoints = 1;

	int attackRange = 1; //Attack range should always be >= 1

	bool isInBattle = false;

	//All directions the Unit can be successfully attacked from.
	AttackDirection attackDirections = AttackDirection::All;

private:
	bool isUnitTurn = false;
	bool attackWindingUp = false;
	int currentAttackNumber = 0;

	std::string skillName;
	std::vector<SkillNode*> activeSkillNodes;

public:
	//All the nodes the unit can move to
	std::vector<GridNode*> movementPathNodes;

	//The end path the unit takes after a call to MoveToNode()
	std::vector<GridNode*> pathNodes;

	XMVECTOR nextMovePos;

	//This is the actor name a Unit is focusing its 'intent' on. It can be another Unit, or Actor.
	std::string actorToFocusOn;

	std::map<std::string, std::unique_ptr<UnitSkill>> skills;

	Unit();
	virtual void Start() override;
	virtual void Tick(float deltaTime) override;
	virtual Properties GetProps() override;

	virtual void InflictDamage(int damage) override;

	void MoveToNode(GridNode* destinationNode);
	void MoveToNode(int x, int y);

	//Figure out movement path and target during battle on turn start
	void StartTurn();
	void EndTurn();

	bool Attack();
	void WindUpAttack();

	void ShowUnitMovementPath();

	void SetUnitLookAt(XMVECTOR lookAtPoint);

private:
	std::vector<GridNode*> GetMovementPathPreviewNodes(GridNode* destinationNode);

	int GetHighestSkillRange();
	bool IsTargetInRangeOfSkills(GridNode* targetNode);
	void ActivateSkill();
	void ClearActiveSkillNodes();
};
