#pragma once
#include "Widget.h"

struct HealthWidget : Widget
{
	int healthPoints = 0;

	virtual void Tick(float deltaTime) override;
};
