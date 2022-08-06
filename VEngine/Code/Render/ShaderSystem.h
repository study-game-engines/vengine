#pragma once

#include <string>
#include <memory>
#include <map>
#include "System.h"
#include "Render/ShaderPair.h"

struct VertexShader;
struct PixelShader;

class ShaderSystem : public System
{
public:
	ShaderSystem() : System("ShaderSystem") {}
	void Init();

	VertexShader* FindVertexShader(const std::wstring filename);
	PixelShader* FindPixelShader(const std::wstring filename);
	ShaderPair FindShaderPair(ShaderPairNames shaderPairNames);

	void ClearShaders();

private:
	void CompileAllShadersFromFile();

	std::map<std::wstring, std::unique_ptr<VertexShader>> vertexShaders;
	std::map<std::wstring, std::unique_ptr<PixelShader>> pixelShaders;
};

extern ShaderSystem shaderSystem;
