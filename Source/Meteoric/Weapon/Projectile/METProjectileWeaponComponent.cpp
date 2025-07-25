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

	TArray<FMETProjectileSpawnHandle> SpawnHandles;
	SpawnHandles.SetNum(NumProjectiles);
	TArray<TWeakObjectPtr<AMETProjectile>> GroupedProjectiles;

	/* Spawn all projectiles in a shot before firing, so we can initialize the grouped projectiles array */
	for (int Index = 0; Index < NumProjectiles; ++Index)
	{
		SpawnProjectile(InParams[Index], SpawnHandles[Index]);
		if (AMETProjectile* Projectile = SpawnHandles[Index].Projectile; ensure(Projectile))
		{
			GroupedProjectiles.Push(Projectile);
		}
	}

	/* Each projectile has a copy of the grouped projectiles array, to keep track of which projectiles are in the same shot */
	for (int Index = 0; Index < NumProjectiles; ++Index)
	{
		if (AMETProjectile* Projectile = SpawnHandles[Index].Projectile; ensure(Projectile))
		{
			Projectile->GroupedProjectiles = GroupedProjectiles;
		}
	}

	/* Once all projectiles are initialized, fire them all at once */
	for (int Index = 0; Index < NumProjectiles; ++Index)
	{
		FireProjectile(SpawnHandles[Index]);
	}
	
	WeaponSpreadComponent->OnWeaponFired();
}

void UMETProjectileWeaponComponent::SpawnProjectile(const FMETSpawnProjectileParams& InParams, FMETProjectileSpawnHandle& OutHandle) const
{
	if (!ensure(WeaponSpreadComponent) || !ensure(ProjectileClass)) return;

	OutHandle.SpawnTransform = WeaponSpreadComponent->GetProjectileSpawnTransform();
	
	if (bPerformSpawnTraceTest)
	{
		OutHandle.bTraceHit = SpawnTraceTest(InParams, OutHandle.SpawnTransform, OutHandle.TraceHitResult);
		if (OutHandle.bTraceHit)
		{
			OutHandle.SpawnTransform.SetLocation(OutHandle.TraceHitResult.Location);
		}
	}
	
	AMETProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMETProjectile>(
		ProjectileClass,
		OutHandle.SpawnTransform,
		InParams.Owner,
		InParams.Instigator,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (ensure(Projectile))
	{
		Projectile->AmmoType = InParams.AmmoType;
		Projectile->ImpactEffectHandle = InParams.ImpactEffectHandle;
		Projectile->DelayedEffectHandle = InParams.DelayedEffectHandle;
		Projectile->bOnlyCollideOnSweep = bPerformSpawnTraceTest;
		Projectile->FinishSpawning(OutHandle.SpawnTransform);
	}

	OutHandle.Projectile = Projectile;
}

void UMETProjectileWeaponComponent::FireProjectile(const FMETProjectileSpawnHandle& InHandle)
{
	AMETProjectile* Projectile = InHandle.Projectile;
	if (!ensure(Projectile)) return;
		
	if (InHandle.bTraceHit)
	{
		Projectile->TriggerImpact(InHandle.TraceHitResult);
	}
	else
	{
		Projectile->Fire(InHandle.SpawnTransform.GetRotation().Vector());
	}
}

bool UMETProjectileWeaponComponent::SpawnTraceTest(const FMETSpawnProjectileParams& InParams, const FTransform& InSpawnTransform, FHitResult& OutHitResult) const
{
	const FVector TraceStart = InSpawnTransform.GetLocation();
	const FVector TraceEnd = TraceStart + InSpawnTransform.GetRotation().Vector() * SpawnTraceDistance;
	const TArray<AActor*> ActorsToIgnore = { GetOwner(), InParams.Instigator };

	EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::None;
#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	if (CVarDrawProjectileDebug.GetValueOnAnyThread() == 1)
	{
		DebugTrace = EDrawDebugTrace::ForDuration;
	}
#endif

	const bool bTraceHit = UKismetSystemLibrary::LineTraceSingle(
	  this, TraceStart, TraceEnd, UEngineTypes::ConvertToTraceType(ECC_Projectile),
	  false, ActorsToIgnore, DebugTrace, OutHitResult, true
	);

	return bTraceHit;
}
