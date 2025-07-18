// Copyright Alex Jobe


#include "METProjectileWeaponComponent.h"

#include "METProjectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/Weapon/Handling/METWeaponSpreadComponent.h"

UMETProjectileWeaponComponent::UMETProjectileWeaponComponent()
	: NumProjectiles(1)
	, bPerformSpawnTraceTest(true)
	, SpawnTraceDistance(200.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UMETProjectileWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	WeaponSpreadComponent = GetOwner()->FindComponentByClass<UMETWeaponSpreadComponent>();
}

void UMETProjectileWeaponComponent::FireProjectiles(const TArray<FMETSpawnProjectileParams>& InParams) const
{
	if (!ensure(InParams.Num() == NumProjectiles)) return;

	for (int Projectile = 0; Projectile < NumProjectiles; ++Projectile)
	{
		SpawnProjectile(InParams[Projectile]);
	}
	
	WeaponSpreadComponent->OnWeaponFired();
}

void UMETProjectileWeaponComponent::SpawnProjectile(const FMETSpawnProjectileParams& InParams) const
{
	if (!ensure(WeaponSpreadComponent) || !ensure(ProjectileClass)) return;

	FTransform SpawnTransform = WeaponSpreadComponent->GetProjectileSpawnTransform();

	bool bTraceHit = false;
	FHitResult TraceHitResult;
	if (bPerformSpawnTraceTest)
	{
		bTraceHit = SpawnTraceTest(InParams, SpawnTransform, TraceHitResult);
		if (bTraceHit)
		{
			SpawnTransform.SetLocation(TraceHitResult.Location);
		}
	}
	
	AMETProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMETProjectile>(
		ProjectileClass,
		SpawnTransform,
		InParams.Owner,
		InParams.Instigator,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (ensure(Projectile))
	{
		Projectile->ImpactDamageHandle = InParams.ImpactDamageHandle;
		Projectile->DelayedDamageHandle = InParams.DelayedDamageHandle;
		Projectile->bOnlyCollideOnSweep = bPerformSpawnTraceTest;
		Projectile->FinishSpawning(SpawnTransform);

		if (bTraceHit)
		{
			Projectile->TriggerImpact(TraceHitResult);
		}
		else
		{
			Projectile->Fire(SpawnTransform.GetRotation().Vector());
		}
	}
}

bool UMETProjectileWeaponComponent::SpawnTraceTest(const FMETSpawnProjectileParams& InParams, const FTransform& InSpawnTransform, FHitResult& OutHitResult) const
{
	const FVector TraceStart = InSpawnTransform.GetLocation();
	const FVector TraceEnd = TraceStart + InSpawnTransform.GetRotation().Vector() * SpawnTraceDistance;
	const TArray<AActor*> ActorsToIgnore = { GetOwner(), InParams.Instigator };

	const bool bTraceHit = UKismetSystemLibrary::LineTraceSingle(
	  this, TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_Projectile),
	  false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true
	);

	return bTraceHit;
}
