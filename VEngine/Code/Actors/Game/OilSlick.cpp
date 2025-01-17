#include "vpch.h"
#include "OilSlick.h"
#include "Components/MeshComponent.h"
#include "FireSource.h"

void OilSlick::Create()
{
	__super::Create();

	DisableAllInteractivity();

	mesh->SetMeshFilename("node.vmesh");
	mesh->SetTexture("oil_slick.png");
}

Properties OilSlick::GetProps()
{
	auto props = __super::GetProps();
	props.title = GetTypeName();
	return props;
}

void OilSlick::Burn()
{
	__super::Burn();

	if (!onFire)
	{
		Transform t;
		t.Decompose(GetWorldMatrix());

		auto fireSource = FireSource::system.Add(t);
		fireSource->Create();
		fireSource->CreateAllComponents();
		fireSource->Start();

		onFire = true;
	}
}
