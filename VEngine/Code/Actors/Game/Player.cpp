#include "Player.h"
#include "Components/MeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/IntuitionComponent.h"
#include "Camera.h"
#include "Input.h"
#include "VMath.h"
#include "VString.h"
#include "Physics/Raycast.h"
#include "Gameplay/GameUtils.h"
#include "Editor/Editor.h"
#include "Actors/Game/NPC.h"
#include "Actors/Game/Pickup.h"
#include "Components/DialogueComponent.h"
#include "Actors/Game/BattleGrid.h"
#include "Actors/Game/GridActor.h"
#include "UI/UISystem.h"
#include "UI/HealthWidget.h"
#include "UI/DialogueWidget.h"
#include "UI/InteractWidget.h"
#include "Gameplay/Intuition.h"
#include "Gameplay/ConditionSystem.h"

DialogueComponent* dialogueComponent;

Player::Player()
{
	nextPos = XMVectorZero();
	nextRot = XMVectorZero();

	mesh = MeshComponent::system.Add(this, MeshComponent("cube.fbx", "wall.png"));
	rootComponent = mesh;

	camera = CameraComponent::system.Add(this, CameraComponent(XMFLOAT3(4.f, 4.f, -9.f), false));

	rootComponent->AddChild(camera);

	widget = WidgetComponent::system.Add(this);
}

void Player::Start()
{
	camera->targetActor = this;

	widget->widget = uiSystem.CreateWidget<DialogueWidget>();
	interactWidget = uiSystem.CreateWidget<InteractWidget>();

	nextPos = GetPositionVector();
	nextRot = GetRotationVector();
}

void Player::Tick(float deltaTime)
{
	ToggleBattleGrid();

	PrimaryAction();

	if (!inConversation && !inInteraction)
	{
		MovementInput(deltaTime);
		RotationInput(deltaTime);
	}
}

Properties Player::GetProps()
{
    auto props = Actor::GetProps();
	props.title = "Player";
	return props;
}

void Player::CreateIntuition(IntuitionComponent* intuitionComponent)
{
	auto intuitionIt = intuitions.find(intuitionComponent->intuitionName);
	if (intuitionIt != intuitions.end())
	{
		editor->Log("%s Intuition already known.", intuitionComponent->intuitionName.c_str());
		return;
	}

	auto intuition = new Intuition();
	intuition->name = intuitionComponent->intuitionName;
	intuition->description = intuitionComponent->intuitionDescription;

	//Check if intuition condition passes
	if (!intuitionComponent->condition.empty())
	{
		intuition->conditionFunc = conditionSystem.FindCondition(intuitionComponent->condition);

		if (intuition->conditionFunc())
		{
			intuitions.emplace(intuition->name, intuition);
			//TODO: change these logs here to Widget->AddToViewport()'s
			editor->Log("%s Intuition created.", intuition->name.c_str());
		}
		else
		{
			delete intuition;
		}
	}
	else
	{
		intuitions.emplace(intuition->name, intuition);
		editor->Log("%s Intuition created.", intuition->name.c_str());
	}
}

void Player::MovementInput(float deltaTime)
{
	const float moveSpeed = 5.5f;
	SetPosition(VMath::VectorConstantLerp(GetPositionVector(), nextPos, deltaTime, moveSpeed));

	if (XMVector4Equal(GetPositionVector(), nextPos) && XMQuaternionEqual(GetRotationVector(), nextRot))
	{
		XMVECTOR previousPos = nextPos;

		if (Input::GetAsyncKey(Keys::W))
		{
			nextPos = GetPositionVector() + GetForwardVectorV();
		}
		if (Input::GetAsyncKey(Keys::S))
		{
			nextPos = GetPositionVector() + -GetForwardVectorV();
		}
		if (Input::GetAsyncKey(Keys::A))
		{
			nextPos = GetPositionVector() + -GetRightVectorV();
		}
		if (Input::GetAsyncKey(Keys::D))
		{
			nextPos = GetPositionVector() + GetRightVectorV();
		}

		if (!XMVector4Equal(previousPos, nextPos))
		{
			Ray ray(this);
			XMVECTOR direction = XMVector3Normalize(nextPos - previousPos);
			if (Raycast(ray, GetPositionVector(), direction, 1.f))
			{
				nextPos = previousPos;
			}
		}
	}
}

void Player::RotationInput(float deltaTime)
{
	const float rotSpeed = 5.0f;
	SetRotation(VMath::QuatConstantLerp(GetRotationVector(), nextRot, deltaTime, rotSpeed));

	if (XMQuaternionEqual(GetRotationVector(), nextRot) && XMVector4Equal(GetPositionVector(), nextPos))
	{
		if (Input::GetKeyUp(Keys::Right))
		{
			constexpr float angle = XMConvertToRadians(90.f);
			nextRot = XMQuaternionMultiply(nextRot, DirectX::XMQuaternionRotationAxis(VMath::XMVectorUp(), angle));
		}
		if (Input::GetKeyUp(Keys::Left))
		{
			constexpr float angle = XMConvertToRadians(-90.f);
			nextRot = XMQuaternionMultiply(nextRot, DirectX::XMQuaternionRotationAxis(VMath::XMVectorUp(), angle));
		}
	}
}

void Player::ToggleBattleGrid()
{
	//toggle battlegrid visibility
	if (Input::GetKeyUp(Keys::Space))
	{
		inCombat = !inCombat;

		auto battleGrid = GameUtils::GetBattleGrid();
		if (battleGrid)
		{
			battleGrid->ToggleActive();
		}

		//toggle all health widgets on
		auto healthWidgets = uiSystem.GetAllWidgetsOfType<HealthWidget>();
		for (auto healthWidget : healthWidgets)
		{
			if (inCombat)
			{
				healthWidget->AddToViewport();
			}
			else
			{
				healthWidget->RemoveFromViewport();
			}
		}
	}
}

void Player::PrimaryAction()
{
	if (Input::GetKeyUp(Keys::Down))
	{
		if (inInteraction)
		{
			interactWidget->RemoveFromViewport();
			inInteraction = false;
			return;
		}

		Ray ray(this);
		if (Raycast(ray, GetPositionVector(), GetForwardVectorV(), 1.5f))
		{
			//PICKUP CHECK
			{
				auto pickup = dynamic_cast<Pickup*>(ray.hitActor);
				if (pickup)
				{
					heldItem = pickup;
					pickup->AddToPlayerInventory();
					return;
				}
			}

			//INTERACT CHECK
			{
				if (!inCombat)
				{
					auto gridActor = dynamic_cast<GridActor*>(ray.hitActor);
					if (gridActor)
					{
						if (gridActor->isInteractable)
						{
							interactWidget->interactText = stows(gridActor->interactText);
							interactWidget->AddToViewport();
							inInteraction = true;

							auto intuition = gridActor->intuition;
							if (intuition->addOnInteract)
							{
								CreateIntuition(gridActor->intuition);
							}
						}
					}
				}
			}

			//DESTRUCTIBLE CHECK
			{
				if (inCombat)
				{
					auto gridActor = dynamic_cast<GridActor*>(ray.hitActor);
					if (gridActor)
					{
						if (gridActor->isDestructible)
						{
							gridActor->health -= 1;
							if (gridActor->health <= 0)
							{
								gridActor->Destroy();
							}

							return;
						}
					}
				}
			}

			//DIALOGUE CHECK
			if (inConversation)
			{
				if (!dialogueComponent->NextLine())
				{
					inConversation = false;
				}
				else
				{
					dialogueComponent->ShowTextAtActor();
				}
			}
			else
			{
				NPC* npc = dynamic_cast<NPC*>(ray.hitActor);
				if (npc)
				{
					dialogueComponent = npc->dialogue;
					inConversation = true;
					dialogueComponent->ShowTextAtActor();
				}
			}
		}
	}
}
