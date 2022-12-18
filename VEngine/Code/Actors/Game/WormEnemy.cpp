#include "vpch.h"
#include "WormEnemy.h"
#include "Components/MeshComponent.h"

WormEnemy::WormEnemy()
{
    auto mesh1 = CreateComponent(MeshComponent("cube.fbx", "test.png"), "Mesh1");
    mesh1->SetScale(0.9f, 0.9f, 0.9f);
    rootComponent->AddChild(mesh1);

    auto mesh2 = CreateComponent(MeshComponent("cube.fbx", "test.png"), "Mesh2");
    mesh2->SetScale(0.9f, 0.9f, 0.9f);
    mesh2->SetPosition(0.f, 1.f, 0.f);
    rootComponent->AddChild(mesh2);

    auto mesh3 = CreateComponent(MeshComponent("cube.fbx", "test.png"), "Mesh3");
    mesh3->SetScale(0.9f, 0.9f, 0.9f);
    mesh3->SetPosition(0.f, 2.f, 0.f);
    rootComponent->AddChild(mesh3);
}

Properties WormEnemy::GetProps()
{
    auto props = __super::GetProps();
    props.title = "WormEnemy";
    return props;
}