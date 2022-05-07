#include "WorldEditor.h"
#include "Physics/Raycast.h"
#include "Input.h"
#include "Editor/Editor.h"
#include "Actors/Actor.h"
#include "Actors/IActorSystem.h"
#include "Editor/TransformGizmo.h"
#include "FileSystem.h"
#include "Camera.h"
#include "Actors/MeshActor.h"
#include "Editor/DebugMenu.h"
#include "Core.h"

WorldEditor worldEditor;

void WorldEditor::Tick()
{
	SpawnActorOnClick();
	HandleActorPicking();
	DuplicateActor();
	DeleteActor();

	SaveWorld();
}

void WorldEditor::HandleActorPicking()
{
	if (transformGizmo.CheckMouseOver() || Core::gameplayOn) 
	{
		return;
	}

	if (Input::GetMouseLeftDown())
	{
		Ray screenPickRay;
		if (RaycastFromScreen(screenPickRay))
		{
			if (Input::GetAsyncKey(Keys::Ctrl) || pickedActors.empty())
			{
				pickedActors.insert(screenPickRay.hitActor);
			}
			else
			{
				pickedActors.clear();
			}

			SetPickedActor(screenPickRay.hitActor);
		}
	}
}

void WorldEditor::DuplicateActor()
{
	if (Input::GetAsyncKey(Keys::Ctrl))
	{
		if (Input::GetKeyDown(Keys::W))
		{
			if (pickedActor)
			{
				pickedActors.clear();

				editor->ClearProperties();

				Transform transform = pickedActor->GetTransform();
				Actor* newDuplicateActor = pickedActor->actorSystem->SpawnActor(transform);

				//The props copying below will overwrite the new actor's name, so keep it here then copy it back.
				const std::string newActorOriginalName = newDuplicateActor->name;

				//Copy values across
				auto oldProps = pickedActor->GetAllProps();
				auto newProps = newDuplicateActor->GetAllProps();
				Properties::CopyProperties(oldProps, newProps);

				newDuplicateActor->CreateAllComponents();

				newDuplicateActor->name = newActorOriginalName;

				editor->SetActorProps(newDuplicateActor);
				editor->UpdateWorldList();

				debugMenu.AddNotification(VString::wformat(
					L"Duplicated new actor [%S]",newActorOriginalName.c_str()));

				//Set new actor as picked in-editor
				pickedActor = newDuplicateActor;
			}
		}
	}
}

void WorldEditor::SaveWorld()
{
	if (Input::GetAsyncKey(Keys::Ctrl))
	{
		if (Input::GetKeyUp(Keys::S))
		{
			FileSystem::WriteAllActorSystems();
		}
	}
}

void WorldEditor::DeleteActor()
{
	if (pickedActor)
	{
		if (Input::GetKeyUp(Keys::Delete))
		{
			editor->RemoveActorFromWorldList();

			if (pickedActors.size() > 1)
			{
				//Destroy all multiple picked actors
				for (auto actor : pickedActors)
				{
					actor->Destroy();
				}
			}
			else
			{
				debugMenu.AddNotification(VString::wformat(
					L"Destroyed actor [%S]", pickedActor->name.c_str()));
				pickedActor->Destroy();
			}

			pickedActors.clear();
			pickedActor = nullptr;

			editor->ClearProperties();
		}
	}
}

//Spawn actor on middle mouse click in viewport
void WorldEditor::SpawnActorOnClick()
{
	if (Input::GetMouseMiddleUp())
	{
		if (spawnSystem)
		{
			Ray ray;
			if (RaycastFromScreen(ray)) //Spawn actor at ray hit point
			{
				XMVECTOR dist = ray.direction * ray.hitDistance;
				XMVECTOR rayEnd = ray.origin + dist;

				Transform transform;

				//Round the position up for spawning on the grid in increments
				rayEnd = XMVectorRound(rayEnd);
				XMStoreFloat3(&transform.position, rayEnd);

				SpawnActor(transform);
			}
			else //Spawn actor a bit in front of the camera based on the click
			{
				XMVECTOR spawnPos = XMLoadFloat3(&activeCamera->transform.position);
				XMFLOAT3 forward = activeCamera->GetForwardVector();
				XMVECTOR forwardVec = XMLoadFloat3(&forward);
				spawnPos += forwardVec * 10.0f;

				XMVECTOR dist = ray.direction * 10.f;
				XMVECTOR rayEnd = ray.origin + dist;

				Transform transform;

				//Round the position up for spawning on the grid in increments
				rayEnd = XMVectorRound(rayEnd);
				XMStoreFloat3(&transform.position, rayEnd);

				SpawnActor(transform);
			}

			editor->UpdateWorldList();
		}
	}
}

void WorldEditor::SpawnActor(Transform& transform)
{
	Actor* actor = nullptr;

	if (!actorTemplateFilename.empty()) //Spawn actor through template
	{
		std::string path = "ActorTemplates/" + actorTemplateFilename;
		Deserialiser d(path, OpenMode::In);

		actor = spawnSystem->SpawnActor(transform);

		auto allActorProps = actor->GetAllProps();
		for (auto& prop : allActorProps)
		{
			d.Deserialise(prop);
		}

		actor->Create();

		//Set the transform, props will have the original transform data and will be
		//different from the click position in world.
		actor->SetTransform(transform);

		std::string newActorName = spawnSystem->name + std::to_string(spawnSystem->GetNumActors());
		actor->SetName(newActorName);

		debugMenu.AddNotification(VString::wformat(
			L"Spawned actor [%S] from template", actor->name.c_str()));
	}
	else //Spawn MeshActor (usually)
	{
		actor = spawnSystem->SpawnActor(transform);
		debugMenu.AddNotification(VString::wformat(
			L"Spawned actor [%S] from MeshActor system", actor->name.c_str()));
	}

	actor->CreateAllComponents();

	pickedActor = actor;
	editor->SetActorProps(pickedActor);
}

void WorldEditor::DeselectPickedActor()
{
	pickedActor = nullptr;
	editor->ClearProperties();
}

void WorldEditor::SetPickedActor(Actor* actor)
{
	assert(actor);
	pickedActor = actor;

	pickedActors.insert(actor);

	editor->SetActorProps(pickedActor);
	editor->SelectActorInWorldList();
}
