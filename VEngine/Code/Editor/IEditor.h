#pragma once

//Base class for Toolkit and Native editors
struct IEditor
{
	void* windowHwnd;

	int viewportMouseX;
	int viewportMouseY;

	int viewportWidth;
	int viewportHeight;

	virtual void Init(int argc, char* argv[]) = 0;
	virtual void Tick() = 0;
	virtual void SetMousePos() = 0;
};
