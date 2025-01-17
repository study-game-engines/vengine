#include "vpch.h"
#include "IncrementalRotator.h"

void IncrementalRotator::Start()
{
	__super::Start();

	actorToRotate = dynamic_cast<GridActor*>(World::GetActorByName(actorNameToRotate));
	SetPlayerFocusGridActor(actorToRotate);
}

Properties IncrementalRotator::GetProps()
{
	auto props = __super::GetProps();
	props.title = GetTypeName();
	props.Add("Actor To Rotate", &actorNameToRotate);
	props.Add("Rotation Increment", &rotationIncrement);
	props.Add("Rotate Direction", &rotateDirection);
	return props;
}

void IncrementalRotator::OnLinkRotateLeft()
{
	XMVECTOR newNextRot = XMQuaternionMultiply(actorToRotate->GetNextRot(),
		DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&rotateDirection),
			XMConvertToRadians(rotationIncrement)));
	actorToRotate->SetNextRot(newNextRot);
}

void IncrementalRotator::OnLinkRotateRight()
{
	XMVECTOR newNextRot = XMQuaternionMultiply(actorToRotate->GetNextRot(),
		DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&rotateDirection),
			XMConvertToRadians(-rotationIncrement)));
	actorToRotate->SetNextRot(newNextRot);
}
