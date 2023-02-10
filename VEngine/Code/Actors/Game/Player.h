#pragma once

#include "../Actor.h"
#include "../ActorSystem.h"

class MeshComponent;
class CameraComponent;
class ComboBarWidget;
class PlayerShieldWidget;

class Player : public Actor
{
public:
	ACTOR_SYSTEM(Player)

	Player();
	virtual void Create() override;
	virtual void Start() override;
	virtual void End() override;
	virtual void Tick(float deltaTime) override;
	virtual Properties GetProps() override;

	void InflictDamage(float damage);

private:
	void MakeOccludingMeshBetweenCameraAndPlayerTransparent();

	void MovementInput();

	//Doubles player's movement speed on a double tap input (only for foward movement. Think of it like a sprint.)
	void DoubleTapDashMovementInput(float deltaTime);

	void RotationInput();

	bool CheckMovementAndRotationHaveStopped();
	bool CheckPlayerWithinLevelBounds();
	bool CheckForObstacle();

	void Shoot();
	void BladeSwipe();
	void ShieldLogic(float deltaTime);
	void Interact();

public:
	MeshComponent* mesh = nullptr;
	CameraComponent* camera = nullptr;

	float shields = 100.f;

private:
	ComboBarWidget* comboBarWidget = nullptr;
	PlayerShieldWidget* playerShieldWidget = nullptr;

	MeshComponent* shieldMesh = nullptr;

	//When to turn shield effect off after interval of damage.
	float shieldCountdownTimer = 0.5f;

	float doubleTapMovementTimer = 0.f;
	bool doubleTapMovementEnabled = false;

	std::vector<Actor*> previousHitTransparentActors;

	XMVECTOR nextPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMVECTOR nextRot = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	float movementSpeed = 10.f;
	float rotationSpeed = 10.f;

	bool allRangeModeActive = true;
	bool inDashBladeAttack = false;

	int damage = 1;
};
