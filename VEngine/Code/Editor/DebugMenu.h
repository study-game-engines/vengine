#pragma once
#include <vector>
#include <string>

struct Properties;

struct DebugNotification
{
	DebugNotification(const wchar_t* note)
	{
		text = note;
		timeOnScreen = 0.f;
	}

	std::wstring text;
	float timeOnScreen;
};

struct DebugMenu
{
	std::vector<DebugNotification> debugNotifications;

	bool fpsMenuOpen = false;
	bool snapMenuOpen = false;
	bool profileMenuOpen = false;
	bool propsMenuOpen = false;

	void Init();
	void Tick(double deltaTime);
	void Cleanup();
	void AddNotification(const wchar_t* note);
	void RenderNotifications(double deltaTime);
	void RenderFPSMenu(double deltaTime);
	void RenderProfileMenu();
	void RenderSnappingMenu();
	void RenderActorProps();
	void IterateOverProperties(Properties& props);
};

extern DebugMenu debugMenu;
