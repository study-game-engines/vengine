#include "ConditionSystem.h"
#include <cassert>

ConditionSystem conditionSystem;

//Global Condition functions
bool TestConditionFunction(std::string arg)
{
	return true;
}

ConditionSystem::ConditionSystem()
{
	ADD_CONDITION(TestConditionFunction);
}

void ConditionSystem::AddCondition(std::string functionName, std::function<bool(std::string)> conditionFunction)
{
	conditions.insert(std::make_pair(functionName, conditionFunction));
}

std::function<bool(std::string)> ConditionSystem::FindCondition(std::string conditionName)
{
	auto conditionIt = conditions.find(conditionName);
	assert(conditionIt != conditions.end());
	return conditionIt->second;
}

bool ConditionSystem::EvaluateCondition(std::string conditionName, std::string conditionArg)
{
	auto conditionIt = conditions.find(conditionName);
	assert(conditionIt != conditions.end());
	return conditionIt->second(conditionArg);
}
