#include "PlayerHealthWidget.h"

void PlayerHealthWidget::Draw()
{
	D2D1_RECT_F rect = AlignLayout(25.f, 25.f, Align::BottomRight);
	for (int i = 0; i < healthPoints; i++)
	{
		rect.left += 50.f * i;
		rect.right += 50.f * i;
		Image("heart_icon.png", rect);
	}
}
