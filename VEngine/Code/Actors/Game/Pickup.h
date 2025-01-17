#pragma once

#include "GridActor.h"

//Actor that is picked up on interaction and slotted as the player's 'held item' as a C++ string.
class Pickup : public GridActor
{
public:
	ACTOR_SYSTEM(Pickup);

	void Create() override;
	void Interact() override;
	Properties GetProps() override;

protected:
	bool canBePickedUp = true;

	//This is what the Pickup is stored as at runtime.
	std::string itemName;
	std::string pickupAudioFilename;

	//The texture icon to display on in-game UI
	std::string itemIconFilename;

private:
	void SetPickupWidgetIcon();
};
