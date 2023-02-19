#pragma once

#include <string>
#include <DirectXMath.h>

using namespace DirectX;

class Actor;
struct SpriteSheet;
class BoxTriggerComponent;
class CameraComponent;

//Utility functions for in-game stuff
namespace GameUtils
{
	void SetActiveCameraTarget(Actor* newTarget);

	void CameraShake(float shake);

	SpriteSheet* SpawnSpriteSheet(std::string textureFilename, XMVECTOR spawnPosition, bool loop, int numRows, int numColumns);

	void PlayAudioOneShot(const std::string audioFilename);

	void SaveGameWorldState();
	void LoadWorld(std::string worldName);

	inline static const std::string gameInstanceSaveFile = "game_instance.sav";

	void SaveGameInstanceData();
	void LoadGameInstanceData();

	//Used with timers to be able to call LoadWorldAndMoveToEntranceTrigger() through Timer.
	extern std::string levelToMoveTo;

	//Move to matching entrance trigger with same name as exited one in loaded world.
	void LoadWorldAndMoveToEntranceTrigger();

	void DisablePlayer();

	void SetActiveCamera(CameraComponent* camera);

	XMVECTOR RandomPointInTriggerNotContainedByMeshBounds(BoxTriggerComponent* boxTrigger);
};
