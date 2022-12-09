#include "vpch.h"
#include "PlayerInputController.h"
#include "Input.h"
#include "Core.h"
#include "Components/CameraComponent.h"
#include "Actors/Game/PlayerUnit.h"
#include "Gameplay/GameUtils.h"

PlayerInputController playerInputController;

void PlayerInputController::Tick(float deltaTime)
{
	if (Core::gameplayOn)
	{
		if (playerUnitToControl)
		{
			playerUnitToControl->ControllerInput(deltaTime);
		}
	}
}

void PlayerInputController::SetPlayerUnitToControl(PlayerUnit* playerUnit)
{
	playerUnitToControl = playerUnit;
	GameUtils::SetActiveCamera(playerUnit->camera);
	GameUtils::SetActiveCameraTarget(playerUnit);
}
