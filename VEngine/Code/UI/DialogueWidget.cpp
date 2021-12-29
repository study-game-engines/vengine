#include "DialogueWidget.h"
#include "VString.h"

void DialogueWidget::Draw()
{
	int sx, sy;
	GetScreenSpaceCoords(sx, sy);

	sy -= 75.f;
	D2D1_RECT_F textRect = CenterLayoutOnScreenSpaceCoords(100.f, 100.f, sx, sy);

	D2D1_RECT_F imageRect = {
		textRect.left - 50.f, textRect.top - 50.f, textRect.right + 50.f, textRect.bottom + 50.f 
	};

	Image("speech_bubble.png", imageRect);
	Text(dialogueText, textRect);
}

void DialogueWidget::SetText(std::string text)
{
	dialogueText = VString::stows(text);
}
