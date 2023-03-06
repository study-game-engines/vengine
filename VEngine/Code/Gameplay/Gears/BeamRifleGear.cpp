#include "vpch.h"
#include "BeamRifleGear.h"
#include "Physics/Raycast.h"
#include "Actors/Game/Player.h"
#include "Actors/Game/Enemy.h"
#include "Gameplay/GameUtils.h"
#include "Gameplay/AttackTypes.h"
#include "Gameplay/GameplayTags.h"
#include "Components/MeshComponent.h"

BeamRifleGear instance;

void BeamRifleGear::Use()
{
	const auto player = Player::system.GetFirstActor();
	const auto damage = player->GetDamage();

	constexpr float beamRange = 100.f;

	const XMVECTOR beamEnd = player->GetPositionV() + (player->GetAimDirection() * beamRange);
	//@Todo: need a better scale down effect here on the beam.
	GameUtils::SpawnPolyboard("Particle/beam_blue.png", player->GetPositionV(), beamEnd, 0.3f);

	HitResult hitResult(player);
	if (Raycast(hitResult, player->GetPositionV(), player->GetAimDirection(), beamRange))
	{
		auto enemy = dynamic_cast<Enemy*>(hitResult.hitActor);
		if (enemy)
		{
			if (enemy->CanBeHit(AttackTypes::Shoot))
			{
				auto mesh = dynamic_cast<MeshComponent*>(hitResult.hitComponent);
				if (mesh)
				{
					if (!mesh->HasTag(GameplayTags::InvincibleMeshPiece))
					{
						//Only inflict damage if mesh can be hit be player attacks
						enemy->InflictDamage(damage);

						mesh->Remove();
					}
				}

				if (enemy->CheckIfAllTaggedMeshesAreDestroyed() || enemy->HasHealthDepleted())
				{
					enemy->OnDestroyed();
				}
			}
		}
	}
}
