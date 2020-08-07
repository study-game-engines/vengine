#pragma once

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include <d2d1_1.h>
#include <dwrite_1.h>
#include <vector>
#include "UIView.h"

class UISystem
{
public:
	void Init();
	void Cleanup(); //D2D1 Actually throws errors if no cleanup
	void Tick();

	std::vector<UIView*> uiViews;

	void PrintMousePos();
	POINT mousePos;

	ID2D1Factory* d2dFactory;
	ID2D1RenderTarget* d2dRenderTarget;
	IDWriteFactory1* writeFactory;
	ID2D1SolidColorBrush* brushText;
	ID2D1SolidColorBrush* brushTextBlack;
	ID2D1SolidColorBrush* brushCloseBox;
	ID2D1SolidColorBrush* brushTransparentMenu;
	ID2D1SolidColorBrush* brushViewBlack;
	ID2D1SolidColorBrush* brushButton;
	IDWriteTextFormat* textFormat;

	int activeUIViewElementIndex = -1;
};

extern UISystem gUISystem;