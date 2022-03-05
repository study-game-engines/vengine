#include "SpriteSheetEmitter.h"
#include "Render/SpriteSystem.h"
#include "Render/TextureSystem.h"
#include "Render/PipelineObjects.h"

SpriteSheetEmitter::SpriteSheetEmitter()
{
}

void SpriteSheetEmitter::Create()
{
	sprite.useSourceRect = true;
	sprite.textureFilename = textureData.filename;
	
	Texture2D* texture = textureSystem.FindTexture2D(textureData.filename);

	int w = texture->desc.Width / numSheetColumns;
	int h = texture->desc.Height / numSheetRows;

	int x = w * currentSheetColumn;
	int y = h * currentSheetRow;

	sprite.srcRect = { x, y, x + w, y + h }; //Only src rect matters for sprite sheets

	spriteSystem.CreateSpriteSheet(sprite);
}

void SpriteSheetEmitter::Tick(float deltaTime)
{
	__super::Tick(deltaTime);

	animationTimer += deltaTime * animationSpeed;
	if (animationTimer > 1.0f)
	{
		animationTimer = 0.f;

		currentSheetColumn++;

		if (currentSheetColumn >= numSheetColumns)
		{
			currentSheetColumn = 0;
			currentSheetRow++;
		}

		//End of animation
		if (currentSheetRow >= numSheetRows)
		{
			currentSheetRow = 0;
			currentSheetColumn = 0;
		}
	}
}

Properties SpriteSheetEmitter::GetProps()
{
	auto props = __super::GetProps();
	props.AddProp(numSheetRows);
	props.AddProp(numSheetColumns);
	props.AddProp(animationSpeed);
	return props;
}

void SpriteSheetEmitter::UpdateSprite()
{
	Texture2D* texture = textureSystem.FindTexture2D(textureData.filename);

	int w = texture->desc.Width / numSheetColumns;
	int h = texture->desc.Height / numSheetRows;

	int x = w * currentSheetColumn;
	int y = h * currentSheetRow;

	sprite.srcRect = { x, y, x + w, y + h };
}
