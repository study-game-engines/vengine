#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
#include "Obj.h"

using namespace DirectX;

enum class EActorSystemID
{
	Base,
	DebugSphere,
	DebugBox
};

class Actor
{
public:
	Actor();
	//virtual void Tick(float deltaTime) = 0;
	XMVECTOR GetPositionVector();
	XMFLOAT3 GetPositionFloat3();
	void SetPosition(XMVECTOR v);
	void SetPosition(float x, float y, float z);
	void SetRotation(XMVECTOR axis, float angle);
	XMMATRIX GetRotation();
	XMFLOAT3 GetScale();
	void SetScale(float x, float y, float z);
	void SetScale(XMVECTOR scale);
	XMVECTOR GetForwardVector();
	XMVECTOR GetRightVector();
	XMVECTOR GetUpVector();
	void Move(float d, XMVECTOR direction);

	XMMATRIX transform = XMMatrixIdentity();

	int vertexBufferOffset;
	bool bRender = true;
};

class ActorSystem
{
public:
	ActorSystem() {}
	//virtual void Tick(float deltaTime) = 0;
	void CreateActors(class RenderSystem* dx, int numActorsToSpawn);
	void AddActor();
	void RemoveActor(int index);

	OBJData modelData;

	struct ID3D11Buffer* vertexBuffer;
	struct ID3D11Buffer* indexBuffer;

	struct ID3D11SamplerState* samplerState;

	ID3D11Resource* texture;
	ID3D11ShaderResourceView* srv;

	BoundingBox boundingBox;
	BoundingSphere boundingSphere;

	UINT64 numVertices; //Frustrum culling is going to make a mess of this 

	std::vector<Actor> actors;

	const wchar_t* shaderName = L"shaders.hlsl";
	const wchar_t* textureName = L"texture.png";
	const char* modelName = "cube.obj";

	EActorSystemID id = EActorSystemID::Base;

	bool bInstancingActors; //bool for setting system to use instancing
};