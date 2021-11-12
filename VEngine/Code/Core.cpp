#include "Core.h"
#include "Input.h"
#include "Render/Renderer.h"
#include "Debug.h"
#include "GameUtils.h"
#include "Camera.h"
#include "Actors/Player.h"
#include "World.h"
#include "UI/UISystem.h"
#include "FileSystem.h"
#include "Audio/AudioSystem.h"
#include "WorldEditor.h"
#include "Render/SpriteSystem.h"

double ticks;
double deltaTime = 0.0;
double deltaAccum;

namespace Core
{
	std::string initialStartingWorldFromEditor;

	bool mainLoop = true;
	bool gameplayOn = false;

	__int64 tickFrequency;
	__int64 frameStartTime;
	__int64 frameEndTime;

	double timeSinceStartup;

	uint32_t frameCount;
	uint32_t finalFrameCount;
}

float Core::GetDeltaTime()
{
	if (deltaTime < 0.0)
	{
		deltaTime = 0.0;
	}
	else if (deltaTime > 1.0)
	{
		//There's a good talk on the, "Myths and Misconceptions of Frame Pacing", by the Serious Sam guys
		//REF:https://www.youtube.com/watch?v=n0zT8YSSFzw
		deltaTime = 1.0f / 60.f;
	}

	return (float)deltaTime;
}

void Core::SetTimerFrequency()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&tickFrequency);
	ticks = 1.0 / (double)tickFrequency;
	deltaAccum = 0.0;
}

void Core::Init()
{
	//For the WIC texture functions from DirectXToolkit and XAudio2 threading
	HR(CoInitializeEx(NULL, COINIT_MULTITHREADED));
	SetTimerFrequency();
}

void Core::Tick()
{
	if (Input::GetAsyncKey(Keys::Ctrl))
	{
		if (Input::GetKeyUp(Keys::P))
		{
			SetGameplayState();
		}
	}

	//End game on Esc pressed
	if (Input::GetKeyDown(Keys::Esc))
	{
		if (gameplayOn)
		{
			EndGame();
			gameplayOn = false;
		}
	}
}

void Core::StartTimer()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&frameStartTime);
}

void Core::EndTimer()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&frameEndTime);
	deltaTime = ticks * (double)(frameEndTime - frameStartTime);

	deltaAccum += deltaTime;
	timeSinceStartup += deltaTime;
	Core::frameCount++;

	if (deltaAccum > 1.0)
	{
		finalFrameCount = Core::frameCount;

		frameCount = 0;
		deltaAccum = 0.0;
	}
}

void Core::StartGame()
{
	gameplayOn = true;

	Player* player = GameUtils::GetPlayer();
	if (player)
	{
		if (player->camera)
		{
			activeCamera = player->camera;
		}
	}

	initialStartingWorldFromEditor = world.worldFilename;

	world.StartAllComponents();
	world.StartAllActors();

	worldEditor.pickedActor = nullptr;
}

void Core::EndGame()
{
	gameplayOn = false;

	uiSystem.RemoveAllWidgets();
	spriteSystem.Reset();

	audioSystem.DeleteLoadedAudioAndChannels();

	//if (initialStartingWorldFromEditor != world.worldFilename)
	{
		fileSystem.LoadWorld(initialStartingWorldFromEditor);
	}

	activeCamera = &editorCamera;

	//Clear the properties dock or you'll end up with pointer problems
	worldEditor.DeselectPickedActor();
}

void Core::SetGameplayState()
{
	gameplayOn = !gameplayOn;

	if (gameplayOn)
	{
		StartGame();
	}
	else if(!gameplayOn)
	{
		EndGame();
	}
}

void Core::HandleWin32MessagePump(UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		Input::SetKeyDown((Keys)wparam);
		break;

	case WM_KEYUP:
		Input::SetKeyUp((Keys)wparam);
		break;

	case WM_MBUTTONUP:
		Input::SetMiddleMouseUp();
		break;	
	
	case WM_MBUTTONDOWN:
		Input::SetMiddleMouseDown();
		break;

	case WM_LBUTTONUP:
		Input::SetLeftMouseUp();
		break;

	case WM_LBUTTONDOWN:
		Input::SetLeftMouseDown();
		break;

	case WM_RBUTTONUP:
		Input::SetRightMouseUp();
		break;

	case WM_RBUTTONDOWN:
		Input::SetRightMouseDown();
		break;

	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wparam) < 0)
		{
			Input::mouseWheelDown = true;
		}
		else
		{
			Input::mouseWheelUp = true;
		}

		break;

	case WM_SIZE:
		switch (wparam)
		{
		case SIZE_MINIMIZED: //swapchain resize messes up if minimised (width & height = 0)
			return;
		}

		UINT resizedWidth = LOWORD(lparam);
		UINT resizedHeight = HIWORD(lparam);
		renderer.ResizeSwapchain(resizedWidth, resizedHeight);

		break;
	}
}

LRESULT __stdcall Core::WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	HandleWin32MessagePump(message, wparam, lparam);
	return DefWindowProc(window, message, wparam, lparam);
}
