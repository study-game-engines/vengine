#include "BattleGrid.h"
#include "Components/InstanceMeshComponent.h"
#include "Render/RenderUtils.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Physics/Raycast.h"
#include "VMath.h"

BattleGrid::BattleGrid()
{
    nodeMesh = InstanceMeshComponent::system.Add(
        this, InstanceMeshComponent(1, "node.fbx", "test.png"));

    nodeMesh->material->shaderFilename = "InstanceShader.hlsl";
    rootComponent = nodeMesh;
}

void BattleGrid::Start()
{
    //This is all pretty lazy, but I need the sizeX and sizeY to determine the buffer sizes on gameplay start
    //and can't do it in the InstanceMeshComponent constructors.
    if (nodeMesh->structuredBuffer)
    {
        nodeMesh->structuredBuffer->Release();
    }
    if (nodeMesh->srv)
    {
        nodeMesh->srv->Release();
    }

    nodeMesh->instanceData.clear();
    nodeMesh->instanceData.push_back(InstanceData());

    int meshInstanceCount = 1;
    nodeMesh->meshInstanceRenderCount = meshInstanceCount;

    //Re-setup shader buffers
    nodeMesh->structuredBuffer = RenderUtils::CreateStructuredBuffer(sizeof(InstanceData) * meshInstanceCount,
        sizeof(InstanceData), nodeMesh->instanceData.data());

    nodeMesh->srv = RenderUtils::CreateSRVForMeshInstance(nodeMesh->structuredBuffer, meshInstanceCount);


    XMMATRIX rootWorldMatrix = rootComponent->GetWorldMatrix();
    XMVECTOR rayOrigin = XMVectorZero();

    nodeMesh->instanceData.clear();
    meshInstanceCount = sizeX * sizeY;

    for (int x = 0; x < sizeX; x++)
    {
        //create grid row
        rows.push_back(GridRow());

        for (int y = 0; y < sizeY; y++)
        {
            rayOrigin = XMVectorSet(x, 10.f, y, 1.f);

            //Set instance model matrix
            InstanceData instanceData = {};
            instanceData.world = rootWorldMatrix;
            nodeMesh->instanceData.push_back(instanceData);

            //raycast against the world to get node position
            Ray ray = {};
            ray.actorsToIgnore.push_back(this);
            if (Raycast(ray, rayOrigin, -VMath::XMVectorUp(), 20.0f))
            {
                //Scale the node down a little
                XMMATRIX scaleMatrix = XMMatrixScaling(0.9f, 0.9f, 0.9f);
                instanceData.world *= scaleMatrix;

                //Position the node at the raycast's hitpos
                XMFLOAT3 hitPos = ray.hitPos;
                hitPos.y += 0.1f;
                XMVECTOR hitPosVector = XMLoadFloat3(&hitPos);
                hitPosVector.m128_f32[3] = 1.0f;

                nodeMesh->instanceData.back().world.r[3] = hitPosVector;
            }

            //create grid node in row
            GridNode node = GridNode(x, y, nodeMesh->instanceData.size());
            rows[x].Add(node);
        }
    }
}

Properties BattleGrid::GetProps()
{
    auto props = Actor::GetProps();
    props.title = "BattleNode";
    props.Add("Size X", &sizeX);
    props.Add("Size Y", &sizeY);
    return props;
}
