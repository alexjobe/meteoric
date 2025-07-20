// Copyright Alex Jobe


#include "METProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/METLogChannels.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"
#include "Sound/SoundCue.h"

AMETProjectile::AMETProjectile()
	: LifeSpan(10.f)
	, DamageDelay(.5f)
	, RocketJumpForce(1000.f)
	, RocketJumpMovementInfluence(0.3f)
	, RocketJumpVerticalBias(0.3f)
	, GroupedProjectileSearchRadius(300.f)
	, bOnlyCollideOnSweep(true)
	, bCollided(false)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComponent->InitSphereRadius(5.f);
	RootComponent = CollisionComponent;

	CollisionComponent->SetCollisionObjectType(ECC_Projectile);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);

	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::CollisionComponent_OnComponentBeginOverlap);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 10000.f;
	ProjectileMovementComponent->MaxSpeed = 10000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AMETProjectile::Fire(const FVector& InDirection) const
{
	if (ensure(ProjectileMovementComponent))
	{
		ProjectileMovementComponent->Velocity = InDirection * ProjectileMovementComponent->InitialSpeed;
	}
}

void AMETProjectile::TriggerImpact(const FHitResult& InHitResult)
{
	AActor* HitActor = InHitResult.GetActor();
	if (!HasAuthority() || !ensure(HitActor) || HitActor == GetInstigator()) return;
	
	if (bCollided) return;

	bCollided = true;
	
	if (ImpactDamageHandle.IsValid())
	{
		UMETAbilitySystemUtils::AddHitResultToEffectSpec(ImpactDamageHandle.EffectHandle, InHitResult);
		Impact(ImpactDamageHandle, InHitResult, HitActor);
	}

	if (DelayedDamageHandle.IsValid())
	{
		UMETAbilitySystemUtils::AddHitResultToEffectSpec(DelayedDamageHandle.EffectHandle, InHitResult);
		StartDelayedDamageTimer(HitActor, InHitResult.GetComponent(), InHitResult);
	}
	else
	{
		Destroy();
	}
}

void AMETProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	if (InAirLoopSound)
	{
		UGameplayStatics::SpawnSoundAttached(InAirLoopSound, GetRootComponent(), NAME_None, FVector(ForceInit),
			FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
	}
}

void AMETProjectile::CollisionComponent_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((bOnlyCollideOnSweep && bFromSweep) || !bOnlyCollideOnSweep)
	{
		TriggerImpact(SweepResult);
	}
}

void AMETProjectile::Explode(const FMETProjectileDamageHandle& InDamage, const FVector& InLocation) const
{
	if (InDamage.ExplosionRadius <= 0.f)
	{
		UE_LOG(LogMET, Warning, TEXT("AMETProjectile::Explode - Explosion radius should be greater than zero"));
		return;
	}

	const EDamageTiming DamageTiming = InDamage.DamageTiming;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	const TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> HitActors;
	
	float ExplosionRadius = InDamage.ExplosionRadius;
	TArray<TStrongObjectPtr<AMETProjectile>> ContributingProjectiles;

	/* If a shot contains multiple explosive projectiles (ex. shotgun), all nearby grouped projectiles contribute to a
	 * single explosion. The final explosion radius is the sum of all contributing projectiles' explosion radii. */
	const bool bClusteredExplosion = ComputeClusteredExplosionContribution(InLocation, DamageTiming, ContributingProjectiles, ExplosionRadius);

	UKismetSystemLibrary::SphereOverlapActors(this, InLocation, ExplosionRadius,
		ObjectTypes, nullptr, ActorsToIgnore, HitActors);

	DrawDebugSphere(GetWorld(), InLocation, ExplosionRadius,
		16, FColor::Red, false, 2.f, 0, 2.f);

	DrawDebugSphere(GetWorld(), InLocation, 2.f,
		16, FColor::Green, false, 2.f, 0, 2.f);
	
	for (const auto& Actor : HitActors)
	{
		if (!ensure(Actor)) continue;

		bool bDied = ApplyDamageEffect(*Actor, InDamage.DamageTiming);
		if (!bDied && bClusteredExplosion)
		{
			bDied = ApplyClusteredDamage(*Actor, ContributingProjectiles, InDamage.DamageTiming);
		}

		if (InDamage.bApplyRocketJumpImpulse && !bDied && Actor == GetInstigator())
		{
			ApplyRocketJumpImpulse(Actor, InLocation, ExplosionRadius);
		}
	}

	// After applying cluster damage to all actors in explosion radius, destroy all contributing projectiles
	for (const auto& StrongProjectile : ContributingProjectiles)
	{
		if (AMETProjectile* Projectile = StrongProjectile.Get())
		{
			Projectile->Destroy();
		}
	}
}

void AMETProjectile::Impact(const FMETProjectileDamageHandle& InDamageHandle, const FHitResult& InHitResult, const AActor* HitActor)
{
	if (!HitActor) return;
	
	if (InDamageHandle.bExplosive)
	{
		Explode(InDamageHandle, InHitResult.ImpactPoint);
	}
	else
	{
		ApplyDamageEffect(*HitActor, InDamageHandle.DamageTiming);
	}

	const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(InHitResult.PhysMaterial.Get());
	Multicast_PlayImpactFX(InHitResult.ImpactPoint, InHitResult.ImpactNormal, SurfaceType);
}

bool AMETProjectile::ApplyDamageEffect(const AActor& InActor, const EDamageTiming& InDamageTiming) const
{
	const FGameplayEffectSpecHandle& EffectHandle = InDamageTiming == EDamageTiming::Impact ? ImpactDamageHandle.EffectHandle : DelayedDamageHandle.EffectHandle;
	if (ensure(EffectHandle.IsValid()))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(&InActor))
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectHandle.Data.Get());
			return AbilitySystemComponent->HasMatchingGameplayTag(METGameplayTags::State_Dead);
		}
	}
	return false;
}

void AMETProjectile::StartDelayedDamageTimer(AActor* InOtherActor, UPrimitiveComponent* InOtherComponent, const FHitResult& InHitResult)
{
	if (!ensure(InOtherActor) || !ensure(InOtherComponent)) return;
	
	ProjectileMovementComponent->StopMovementImmediately();
	if (InHitResult.IsValidBlockingHit())
	{
		SetActorLocation(InHitResult.ImpactPoint);
	}
	
	AttachToComponent(InOtherComponent, FAttachmentTransformRules::KeepWorldTransform, InHitResult.BoneName);

	FTimerDelegate DamageTimerDelegate;
	DamageTimerDelegate.BindUFunction(this, FName("ApplyDelayedDamage"), InOtherActor, InHitResult);
	GetWorldTimerManager().SetTimer(DamageTimerHandle, DamageTimerDelegate, DamageDelay, false);
}

void AMETProjectile::ApplyDelayedDamage(const AActor* InActor, const FHitResult& InHitResult)
{
	if (!ensure(IsValid(InActor))) return;

	Impact(DelayedDamageHandle, InHitResult, InActor);

	Destroy();
}

void AMETProjectile::ApplyRocketJumpImpulse(const AActor* InActor, const FVector& InLocation, const float InExplosionRadius) const
{
	if (UCharacterMovementComponent* MovementComponent = InActor ? InActor->FindComponentByClass<UCharacterMovementComponent>() :  nullptr)
	{
		FVector ExplosionToPlayer = InActor->GetActorLocation() - InLocation;
		ExplosionToPlayer.Z = FMath::Max(ExplosionToPlayer.Z, 1.0f); // Vertical force bias
		ExplosionToPlayer.Normalize();
		
		/*const float PlayerSpeed = MovementComponent->Velocity.Size();
		const float MaxPlayerSpeed = MovementComponent->GetMaxSpeed();
		const float InfluenceScale = FMath::Clamp(PlayerSpeed / MaxPlayerSpeed, 0.f, 1.f);
		MovementInfluence *= InfluenceScale;*/
		
		const FVector MovementDirection = MovementComponent->Velocity.GetSafeNormal();
		FVector FinalImpulseDirection = (ExplosionToPlayer * (1.0f - RocketJumpMovementInfluence)) + (MovementDirection * RocketJumpMovementInfluence);
		DrawDebugDirectionalArrow(GetWorld(), InLocation, InLocation + FinalImpulseDirection * 100.f, 2.f, FColor::Cyan, false, 5.f, 0, 2.f);
		FinalImpulseDirection.Z += RocketJumpVerticalBias;
		FinalImpulseDirection.Normalize();
		DrawDebugDirectionalArrow(GetWorld(), InLocation, InLocation + FinalImpulseDirection * 100.f, 2.f, FColor::Green, false, 5.f, 0, 2.f);

		/*const float DistanceSquared = FVector::DistSquared(InLocation, InActor->GetActorLocation());
		const float RadiusSquared = FMath::Square(InExplosionRadius);
		const float ForceScale = FMath::Clamp(1.f - (DistanceSquared / RadiusSquared), 0, 1); // Falloff
		const float KnockbackStrength = 1000.f * ForceScale;*/

		MovementComponent->AddImpulse(FinalImpulseDirection * RocketJumpForce, true);

		//DrawDebugSphere(GetWorld(), InLocation, InExplosionRadius, 16, FColor::Magenta, false, 2.f, 0, 2.f);
	}
}

bool AMETProjectile::ComputeClusteredExplosionContribution(const FVector& InExplosionOrigin, const EDamageTiming& InDamageTiming, TArray<TStrongObjectPtr<AMETProjectile>>& OutContributingProjectiles, float& OutExplosionRadius) const
{
	bool bClusteredExplosion = false;
	if (GroupedProjectiles.Num() > 1)
	{
		for (auto& WeakProjectile : GroupedProjectiles)
		{
			auto StrongProjectile = WeakProjectile.Pin();
			if (AMETProjectile* Projectile = StrongProjectile.Get(); Projectile != nullptr && Projectile != this && !Projectile->bCollided)
			{
				if (FVector::DistSquared(Projectile->GetActorLocation(), InExplosionOrigin) < FMath::Square(GroupedProjectileSearchRadius))
				{
					bClusteredExplosion = true;
					Projectile->bCollided = true;
					OutContributingProjectiles.Push(StrongProjectile);
					OutExplosionRadius += InDamageTiming == EDamageTiming::Impact
															? Projectile->ImpactDamageHandle.ExplosionRadius
															: Projectile->DelayedDamageHandle.ExplosionRadius;
				}
			}
		}
	}
	return bClusteredExplosion;
}

bool AMETProjectile::ApplyClusteredDamage(const AActor& InActor, TArray<TStrongObjectPtr<AMETProjectile>>& InContributingProjectiles, const EDamageTiming& InDamageTiming)
{
	bool bDied = false;
	for (const auto& StrongProjectile : InContributingProjectiles)
	{
		if (const AMETProjectile* Projectile = StrongProjectile.Get(); ensure(Projectile))
		{
			bDied = Projectile->ApplyDamageEffect(InActor, InDamageTiming);
		}
		if (bDied) break;
	}
	return bDied;
}

void AMETProjectile::Multicast_PlayImpactFX_Implementation(FVector_NetQuantize ImpactPoint, FVector_NetQuantizeNormal ImpactNormal, EPhysicalSurface SurfaceType)
{
	if (ImpactVfx)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVfx, ImpactPoint, ImpactNormal.Rotation());
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactPoint, ImpactNormal.Rotation());
	}
}
