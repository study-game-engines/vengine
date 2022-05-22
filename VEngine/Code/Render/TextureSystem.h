#pragma once
#include <unordered_map>
#include <string>
#include "System.h"

class Texture2D;

//Maps and manages various texture (2D, 3D, etc.) formats to their filenames on disk.
struct TextureSystem : System
{
	std::unordered_map<std::string, Texture2D*> texture2DMap;

	std::wstring selectedTextureInEditor;

	TextureSystem();
	void AddTexture2D(Texture2D* texture);
	Texture2D* FindTexture2D(std::string textureFilename);
	void CreateAllTextures();
	void Cleanup();
};

extern TextureSystem textureSystem;
