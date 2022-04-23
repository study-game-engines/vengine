#include "DiffuseProbeMap.h"
#include "Components/InstanceMeshComponent.h"

DiffuseProbeMap::DiffuseProbeMap()
{
	//Set mesh count as 1
	instanceMeshComponent = InstanceMeshComponent::system.Add(this, InstanceMeshComponent(1, "cube.fbx", "test.png"));
	instanceMeshComponent->SetShaderName("InstanceShader.hlsl");
	rootComponent = instanceMeshComponent;
}

void DiffuseProbeMap::Create()
{
	instanceMeshComponent->SetInstanceCount(GetProbeCount());
	SetInstanceMeshData();
}

Properties DiffuseProbeMap::GetProps()
{
	auto props = __super::GetProps();
	props.title = "DiffuseProbeMap";
	props.AddProp(sizeX);
	props.AddProp(sizeY);
	props.AddProp(sizeZ);
	return props;
}

void DiffuseProbeMap::SetInstanceMeshData()
{
	std::vector<InstanceData> instanceData;

	for (int x = 0; x < sizeX; x++)
	{
		probeData.data.push_back(std::vector<std::vector<XMFLOAT4>>());

		for (int y = 0; y < sizeY; y++)
		{
			probeData.data[x].push_back(std::vector<XMFLOAT4>());

			for (int z = 0; z < sizeZ; z++)
			{
				InstanceData data = {};
				data.world = XMMatrixTranslation((float)x, (float)y, (float)z);

				data.world.r[0].m128_f32[0] = 0.15f;
				data.world.r[1].m128_f32[1] = 0.15f;
				data.world.r[2].m128_f32[2] = 0.15f;

				instanceData.push_back(data);

				probeData.data[x][y].push_back(data.colour);
			}
		}
	}

	instanceMeshComponent->SetInstanceData(instanceData);
}

void DiffuseProbeMap::SetProbeColour(XMFLOAT3 colour, uint32_t instanceMeshIndex)
{
}

uint32_t DiffuseProbeMap::GetProbeCount()
{
	return sizeX * sizeY * sizeZ;
}

XMFLOAT4 DiffuseProbeMap::GetProbe(int x, int y, int z)
{
	return probeData.data[x][y][z];
}

XMFLOAT4 DiffuseProbeMap::FindClosestProbe(XMVECTOR pos)
{
	std::map<float, XMFLOAT4> distanceMap;

	for (auto& probe : instanceMeshComponent->instanceData)
	{
		distanceMap[XMVector3Length(probe.world.r[3] - pos).m128_f32[0]] = probe.colour;
	}

	return distanceMap.begin()->second;
}
