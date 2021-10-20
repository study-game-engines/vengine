#pragma once
#include "SpatialComponent.h"
#include "ComponentSystem.h"
#include <string>

struct AudioComponent : SpatialComponent
{
	COMPONENT_SYSTEM(AudioComponent)

	std::string audioFilename;

	uint64_t channelID = 0;

	float volume = 1.0f;
	float pitch = 1.0f;

	bool playOnStart = false;
	bool loop = false;

	AudioComponent();
	virtual void Tick(float deltaTime) override;
	virtual void Start() override;
	virtual void Create() override;
	virtual Properties GetProps() override;

	void Play();
	void Stop();
};
