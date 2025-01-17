#pragma once

#include "Components/SpatialComponent.h"
#include "Components/ComponentSystem.h"
#include "Render/MeshData.h"
#include "Physics/Raycast.h"

struct Buffer;
struct MeshDataProxy;

//Taken from 'Mathematics for 3D Game Programming and Computer Graphics', Chapter 9.3.3: Polyboards.
//Polyboard is a line of 'planes' facing the camera for line-like effects (beams, lightning, etc.)
class Polyboard : public SpatialComponent
{
public:
	COMPONENT_SYSTEM(Polyboard);

	Polyboard();
	void Create() override;
	void Tick(float deltaTime) override;
	virtual Properties GetProps() override;

	void CalcVertices();

	void SetStartPoint(const XMVECTOR start);
	void SetEndPoint(const XMVECTOR end);
	bool RaycastFromStartToEndPoints(HitResult& hit);
	void SetDestroyTimer(float timerMax);

	Buffer& GetVertexBuffer() { return *vertexBuffer; }
	Buffer& GetIndexBuffer() { return *indexBuffer; }

	auto& GetVertices() { return vertices; }
	auto& GetIndices() { return indices; }

	void SetTextureFilename(const std::string_view filename) { textureData.filename = filename; }
	std::string GetTextureFilename() { return textureData.filename; }

protected:
	void GenerateVertices();

	TextureData textureData;

	Buffer* vertexBuffer = nullptr;
	Buffer* indexBuffer = nullptr;

	std::vector<Vertex> vertices;
	std::vector<MeshData::indexDataType> indices;

	XMFLOAT3 startPoint;
	XMFLOAT3 endPoint;

	float radius = 0.8f;

	float destroyTimer = 0.f;
	float destroyTimerMax = 0.f;

	bool setToDestroy = false;
};
