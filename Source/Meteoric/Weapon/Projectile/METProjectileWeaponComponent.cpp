// Copyright Alex Jobe


#include "METProjectileWeaponComponent.h"

#include "METProjectile.h"

UMETProjectileWeaponComponent::UMETProjectileWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETProjectileWeaponComponent::FireProjectile(const FMETSpawnProjectileParams& Params) const
{
	if (!ensure(ProjectileClass) || !ensure(Params.Owner)) return;
	
	AMETProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMETProjectile>(
		ProjectileClass,
		Params.SpawnTransform,
		Params.Owner,
		Params.Instigator,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (ensure(Projectile))
	{
		Projectile->ImpactDamageEffectHandle = Params.ImpactDamageEffectHandle;
		Projectile->DelayedDamageEffectHandle = Params.DelayedDamageEffectHandle;
		Projectile->FinishSpawning(Params.SpawnTransform);
		Projectile->Fire(Params.SpawnTransform.GetRotation().Vector());
	}
}