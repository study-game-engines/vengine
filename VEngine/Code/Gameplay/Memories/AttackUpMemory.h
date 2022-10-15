#pragma once

#include "../Memory.h"

struct AttackUpMemory : public Memory
{
	AttackUpMemory() : 
		Memory("Attack Up",
		"Increases attack by 1",
		"",
		"",
		1, 0) {}
};

extern AttackUpMemory attackUpMemory;
