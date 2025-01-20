// Copyright Alex Jobe


#include "METProjectileWeaponComponent.h"

#include "METProjectile.h"
#include "Meteoric/Weapon/Handling/METWeaponSpreadComponent.h"

UMETProjectileWeaponComponent::UMETProjectileWeaponComponent()
	: NumProjectiles(1)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UMETProjectileWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	WeaponSpreadComponent = GetOwner()->FindComponentByClass<UMETWeaponSpreadComponent>();
}

void UMETProjectileWeaponComponent::FireProjectiles(const TArray<FMETSpawnProjectileParams>& Params) const
{
	if (!ensure(Params.Num() == NumProjectiles)) return;

	for (int Projectile = 0; Projectile < NumProjectiles; ++Projectile)
	{
		SpawnProjectile(Params[Projectile]);
	}
	
	WeaponSpreadComponent->OnWeaponFired();
}

void UMETProjectileWeaponComponent::SpawnProjectile(const FMETSpawnProjectileParams& Params) const
{
	if (!ensure(WeaponSpreadComponent) || !ensure(ProjectileClass)) return;

	const FTransform SpawnTransform = WeaponSpreadComponent->GetProjectileSpawnTransform();
	
	AMETProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMETProjectile>(
		ProjectileClass,
		SpawnTransform,
		Params.Owner,
		Params.Instigator,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (ensure(Projectile))
	{
		Projectile->ImpactDamageEffectHandle = Params.ImpactDamageEffectHandle;
		Projectile->DelayedDamageEffectHandle = Params.DelayedDamageEffectHandle;
		Projectile->FinishSpawning(SpawnTransform);
		Projectile->Fire(SpawnTransform.GetRotation().Vector());
	}
}
