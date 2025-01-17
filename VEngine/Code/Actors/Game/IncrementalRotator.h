#pragma once

#include "GridActor.h"

class IncrementalRotator : public GridActor
{
public:
	ACTOR_SYSTEM(IncrementalRotator);

	void Start() override;
	Properties GetProps() override;

	void OnLinkRotateLeft() override;
	void OnLinkRotateRight() override;

private:
	std::string actorNameToRotate;
	GridActor* actorToRotate = nullptr;
	XMFLOAT3 rotateDirection = XMFLOAT3(0.f, 1.f, 0.f);
	float rotationIncrement = 90.f; //In degrees
};
