#pragma once
#include "../Widget.h"

struct Memory;

//Draws all current known Memories the player has in a game setting.
struct MemoryMenuWidget : Widget
{
	Memory* selectedMemory = nullptr;

	virtual void Draw(float deltaTime) override;
};
