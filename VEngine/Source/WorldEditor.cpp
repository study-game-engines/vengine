#include "WorldEditor.h"
#include "Input.h"
#include "Camera.h"
#include "UISystem.h"
#include "World.h"
#include "CoreSystem.h"
#include "ShaderFactory.h"
#include "MathHelpers.h"
#include "..\EditorMainWindow.h"
#include "PropertiesDock.h"
#include "EditorSystem.h"

WorldEditor gWorldEditor;

void WorldEditor::Tick(ID3D11Buffer* debugLinesBuffer)
{
	if (gUISystem.bUIClicked)
	{
		return;
	}

	World* world = GetWorld();

	if (pickedActor)
	{
		//Transform gizmo on button presses (i.e using arrows for rotation etc.)

		//ROTATION
		if (gInputSystem.GetKeyUpState('R'))
		{
			bRotateActors = !bRotateActors;
			bTranslateActors = false;
			bScaleActors = false;
		}

		if (bRotateActors)
		{
			if (gInputSystem.GetAsyncKey(Keys::Right))
			{
				rotateSpeed += 1.0f * gCoreSystem.deltaTime;
				pickedActor->SetRotation(XMVectorUp(), rotateSpeed);
			}
			else
			{
				rotateSpeed = 0.f;
			}
		}

		//SCALE
		if (gInputSystem.GetKeyUpState('E'))
		{
			bScaleActors = ! bScaleActors;
			bRotateActors = false;
			bTranslateActors = false;
		}

		if (bScaleActors)
		{
			const float deltaTime = gCoreSystem.deltaTime;

			if (gInputSystem.GetAsyncKey(Keys::Up))
			{
				XMFLOAT3 scale = pickedActor->GetScale();
				scale.x += scaleSpeed * deltaTime;
				scale.y += scaleSpeed * deltaTime;
				scale.z += scaleSpeed * deltaTime;
				pickedActor->SetScale(scale);
			}
			else if (gInputSystem.GetAsyncKey(Keys::Down))
			{
				XMFLOAT3 scale = pickedActor->GetScale();
				if (scale.x > 0.f)
				{
					scale.x -= scaleSpeed * deltaTime;
					scale.y -= scaleSpeed * deltaTime;
					scale.z -= scaleSpeed * deltaTime;
					pickedActor->SetScale(scale);
				}
			}
		}

		//TRANSLATION
		/*if (gInputSystem.GetKeyUpState('W'));
		{
			bTranslateActors = !bTranslateActors;
			bScaleActors = false;
			bRotateActors = false;
		}*/

		if (bTranslateActors)
		{
			if (gInputSystem.GetAsyncKey(Keys::Ctrl))
			{
				if (gInputSystem.GetKeyDownState(VK_UP))
				{
					pickedActor->Move(moveIncrement, XMVectorUp());
				}
				else if (gInputSystem.GetKeyDownState(VK_DOWN))
				{
					pickedActor->Move(-moveIncrement, XMVectorUp());
				}
			}
			else if (gInputSystem.GetKeyDownState(VK_UP))
			{
				pickedActor->Move(moveIncrement, XMVectorForward());
			}
			else if (gInputSystem.GetKeyDownState(VK_DOWN))
			{
				pickedActor->Move(-moveIncrement, XMVectorForward());
			}
			else if (gInputSystem.GetKeyDownState(VK_LEFT))
			{
				pickedActor->Move(-moveIncrement, XMVectorRight());
			}
			else if (gInputSystem.GetKeyDownState(VK_RIGHT))
			{
				pickedActor->Move(moveIncrement, XMVectorRight());
			}
		}

		//Set properties widget info
		{
			/*PropertiesWidget* propWidget = editorMainWindow->propertiesDock->propWidget;

			XMFLOAT3 pos = pickedActor->GetPositionFloat3();
			propWidget->posEditX->setText(QString::number(pos.x));
			propWidget->posEditY->setText(QString::number(pos.y));
			propWidget->posEditZ->setText(QString::number(pos.z));

			XMFLOAT3 scale = pickedActor->GetScale();
			propWidget->scaleEditX->setText(QString::number(scale.x));
			propWidget->scaleEditY->setText(QString::number(scale.y));
			propWidget->scaleEditZ->setText(QString::number(scale.z));

			XMFLOAT3 rot = pickedActor->GetRollPitchYaw();
			propWidget->rotEditX->setText(QString::number(rot.x));
			propWidget->rotEditY->setText(QString::number(rot.y));
			propWidget->rotEditZ->setText(QString::number(rot.z));*/
		}

		//Set translation widget positions (Never going to have more than 3, vector accesses are alright)
		translationGizmos.actors[0]->SetPosition(pickedActor->GetPositionVector());
		translationGizmos.actors[1]->SetPosition(pickedActor->GetPositionVector());
		translationGizmos.actors[2]->SetPosition(pickedActor->GetPositionVector());

		//Delete actors in editor
		if (gInputSystem.GetKeyUpState(VK_DELETE))
		{
			world->GetActorSystem(screenPickRay.actorSystemIndex)->RemoveActor(screenPickRay.actorIndex);
			pickedActor = nullptr;
		}
	}

	//Move actor along translation gizmo axis
	if (pickedAxis)
	{
		//MoveActor(pickedActor, pickedAxis->GetRightVector());
		if (pickedAxis->pickedAxis == EPickedAxis::X)
		{
			MoveActor(pickedActor, EPickedAxis::X);
		}
		else if (pickedAxis->pickedAxis == EPickedAxis::Y)
		{
			MoveActor(pickedActor, EPickedAxis::Y);
		}
		else if (pickedAxis->pickedAxis == EPickedAxis::Z)
		{
			MoveActor(pickedActor, EPickedAxis::Z);
		}
	}


	//Actor picking for editor
	if (gInputSystem.GetMouseLeftUpState())
	{
		pickedAxis = nullptr;
	}
	else if (gInputSystem.GetMouseLeftDownState() && !gUISystem.bUIClicked)
	{
		lastMousePosX = gUISystem.mousePos.x;
		lastMousePosY = gUISystem.mousePos.y;

		if (RaycastAllFromScreen(screenPickRay, gUISystem.mousePos.x, gUISystem.mousePos.y, &editorCamera, GetWorld()))
		{
			if (RaycastTriangleIntersect(screenPickRay))
			{
				pickedDirection = XMLoadFloat3(&screenPickRay.normal);
				DebugPrint("%f %f %f\n", screenPickRay.normal.x, screenPickRay.normal.y, screenPickRay.normal.z);
			}

			if (debugLinesBuffer)
			{
				DrawRayDebug(screenPickRay.origin, screenPickRay.direction, screenPickRay.distance, debugLinesBuffer);
			}

			translationGizmos.actors[0]->bRender = true;
			translationGizmos.actors[1]->bRender = true;
			translationGizmos.actors[2]->bRender = true;

			actorIndex = screenPickRay.actorIndex;
			actorSystemIndex = screenPickRay.actorSystemIndex;

			if (pickedActor)
			{
				//Properties Dock Panel
				{
					ActorSystem* actorSystemToDisplay = GetWorld()->GetActorSystem(actorSystemIndex);
					gEditorSystem->DisplayActorSystemProperties(pickedActor);
				}

				pickedActor->bPicked = true;
			}
		}
		else
		{
			if (pickedActor)
			{
				pickedActor->bPicked = false;
				//pickedActor = nullptr;
				
				translationGizmos.actors[0]->bRender = false;
				translationGizmos.actors[1]->bRender = false;
				translationGizmos.actors[2]->bRender = false;
			}
		}
	}
}

void WorldEditor::Init()
{
	//Init translation gizmos
	translationGizmos.shaderName = L"depthDraw.hlsl";
	translationGizmos.name = L"Translation Gizmos";
	translationGizmos.modelName = "x_axis.fbx";

	translationGizmos.CreateActors<TranslationGizmo>(&gRenderSystem, 3);

	translationGizmos.GetActor(0)->material.baseColour = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
	TranslationGizmo* xGizmo = dynamic_cast<TranslationGizmo*>(translationGizmos.GetActor(0));
	xGizmo->pickedAxis = EPickedAxis::X;

	translationGizmos.GetActor(1)->SetRotation(90.f, 0.f, 0.f);
	translationGizmos.GetActor(1)->material.baseColour = XMFLOAT4(0.f, 1.f, 0.f, 1.f);
	TranslationGizmo* yGizmo = dynamic_cast<TranslationGizmo*>(translationGizmos.GetActor(1));
	yGizmo->pickedAxis = EPickedAxis::Y;

	translationGizmos.GetActor(2)->SetRotation(0.f, 90.f, 0.f);
	translationGizmos.GetActor(2)->material.baseColour = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
	TranslationGizmo* zGizmo = dynamic_cast<TranslationGizmo*>(translationGizmos.GetActor(2));
	zGizmo->pickedAxis = EPickedAxis::Z;

	//translationGizmos.bRender = false;
}

void WorldEditor::MoveActor(Actor* actor, XMVECTOR direction)
{
	int mouseX = gUISystem.mousePos.x;
	int mouseY = gUISystem.mousePos.y;

	if (gInputSystem.GetAsyncKey(Keys::LeftMouse))
	{
		SetCapture(gCoreSystem.mainWindow);

		float dx = XMConvertToRadians(0.25f * (float)(mouseX - lastMousePosX));
		float dy = XMConvertToRadians(0.25f * (float)(mouseY - lastMousePosY));

		if (actor)
		{
			XMVECTOR up = XMVectorEqual(direction, XMVectorUp());
			//if (up.m128_f32[0] == 0)
			{
				actor->Move((dy + dx) * pickedActorMoveSpeed, direction);
			}
		}

		ReleaseCapture();
	}

	lastMousePosX = mouseX;
	lastMousePosY = mouseY;
}

//Actors as the movement along the translation axis using mouse (Different from Actor::Move())
void WorldEditor::MoveActor(Actor* actor, EPickedAxis axis)
{
	int mouseX = gUISystem.mousePos.x;
	int mouseY = gUISystem.mousePos.y;

	if (gInputSystem.GetAsyncKey(Keys::LeftMouse))
	{
		SetCapture(gCoreSystem.mainWindow);

		float dx = XMConvertToRadians(0.25f * (float)(mouseX - lastMousePosX));
		float dy = XMConvertToRadians(0.25f * (float)(mouseY - lastMousePosY));

		if (bMoveActorsInIncrements && actor)
		{
			//For incremental movement
			if (pickedAxis->pickedAxis == EPickedAxis::X)
			{
				dxAccum += dx;
				if (dxAccum > 0.5f)
				{
					actor->Move(moveIncrement, XMVectorSet(1.f, 0.f, 0.f, 0.f));
					dxAccum = 0;
				}
				else if (dxAccum < -0.5f)
				{
					actor->Move(-moveIncrement, XMVectorSet(1.f, 0.f, 0.f, 0.f));
					dxAccum = 0;
				}
			}

			if (pickedAxis->pickedAxis == EPickedAxis::Y)
			{
				dyAccum += dy;
				if (dyAccum > 0.5f)
				{
					actor->Move(-moveIncrement, XMVectorSet(0.f, 1.f, 0.f, 0.f));
					dyAccum = 0;
				}
				else if (dyAccum < -0.5f)
				{
					actor->Move(moveIncrement, XMVectorSet(0.f, 1.f, 0.f, 0.f));
					dyAccum = 0;
				}
			}

			if (pickedAxis->pickedAxis == EPickedAxis::Z)
			{
				dzAccum += dx;
				if (dzAccum > 0.5f)
				{
					actor->Move(-moveIncrement, XMVectorSet(0.f, 0.f, 1.f, 0.f));
					dzAccum = 0;
				}
				else if (dzAccum < -0.5f)
				{
					actor->Move(moveIncrement, XMVectorSet(0.f, 0.f, 1.f, 0.f));
					dzAccum = 0;
				}
			}
		}
		else if (actor)
		{
			if (axis == EPickedAxis::X)
			{
				//these if()s are here to do correct translation based on camera pos
				if (editorCamera.location.m128_f32[2] > pickedActor->GetPositionFloat3().z) 
				{
					actor->Move(-dx * pickedActorMoveSpeed, XMVectorRight());
				}
				else
				{
					actor->Move(dx * pickedActorMoveSpeed, XMVectorRight());
				}
			}
			else if (axis == EPickedAxis::Y)
			{
				actor->Move(-dy * pickedActorMoveSpeed, XMVectorUp());
			}	
			else if (axis == EPickedAxis::Z)
			{
				if (editorCamera.location.m128_f32[0] > pickedActor->GetPositionFloat3().x)
				{
					actor->Move(dx * pickedActorMoveSpeed, XMVectorForward());
				}
				else
				{
					actor->Move(-dx * pickedActorMoveSpeed, XMVectorForward());
				}
			}
		}

		if (actor)
		{
			translationGizmos.actors[0]->SetPosition(actor->GetPositionVector());
			translationGizmos.actors[1]->SetPosition(actor->GetPositionVector());
			translationGizmos.actors[2]->SetPosition(actor->GetPositionVector());
		}

		ReleaseCapture();
	}

	lastMousePosX = mouseX;
	lastMousePosY = mouseY;
}
