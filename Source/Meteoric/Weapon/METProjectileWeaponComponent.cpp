// Copyright Alex Jobe


#include "METProjectileWeaponComponent.h"

#include "METProjectile.h"

UMETProjectileWeaponComponent::UMETProjectileWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETProjectileWeaponComponent::FireProjectile(const FTransform& InSpawnTransform, AActor* InOwner, APawn* InInstigator, const FGameplayEffectSpecHandle& InDamageEffect) const
{
	if (!ensure(ProjectileClass) || !ensure(InOwner)) return;
	
	AMETProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMETProjectile>(ProjectileClass, InSpawnTransform, InOwner, InInstigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (ensure(Projectile))
	{
		Projectile->DamageEffectHandle = InDamageEffect;
		Projectile->FinishSpawning(InSpawnTransform);
		Projectile->Fire(InSpawnTransform.GetRotation().Vector());
	}
}