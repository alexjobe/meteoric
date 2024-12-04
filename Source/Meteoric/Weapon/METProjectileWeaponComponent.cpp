// Copyright Alex Jobe


#include "METProjectileWeaponComponent.h"

#include "METProjectile.h"

UMETProjectileWeaponComponent::UMETProjectileWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETProjectileWeaponComponent::FireProjectile(const FTransform& InSpawnTransform, APawn* InOwner) const
{
	if (!ensure(ProjectileClass) || !ensure(InOwner)) return;
	
	AMETProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMETProjectile>(ProjectileClass, InSpawnTransform, InOwner, InOwner, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (ensure(Projectile))
	{
		Projectile->FinishSpawning(InSpawnTransform);
		Projectile->Fire(InSpawnTransform.GetRotation().Vector());
	}
}