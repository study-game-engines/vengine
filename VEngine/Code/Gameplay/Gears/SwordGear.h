#pragma once

#include "Gear.h"

class SwordGear : public Gear
{
public:
	SwordGear() : Gear("SwordGear") {}
	void Use() override;
};