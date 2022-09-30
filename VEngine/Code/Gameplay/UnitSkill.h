#pragma once

//A skill with a grid range and effect a unit in level can perform.
class UnitSkill
{
protected:
	enum class SkillType
	{
		Damage
	};

	int range = 1;
	SkillType skillType = SkillType::Damage;

public:
	//Spawn AOE esque nodes for every grid node in range of this skill.
	virtual void SetNodesForSkillRange(int unitIndexX, int unitIndexY) = 0;

	int GetRange() { return range; }
};
