// Copyright Alex Jobe


#include "METProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/METLogChannels.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"

AMETProjectile::AMETProjectile()
	: LifeSpan(10.f)
	, DamageDelay(.5f)
	, RocketJumpForce(1000.f)
	, RocketJumpMovementInfluence(0.3f)
	, RocketJumpVerticalBias(0.3f)
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

		if (ImpactDamageHandle.bExplosive)
		{
			Explode(ImpactDamageHandle, InHitResult.ImpactPoint);
		}
		else
		{
			ApplyDamageEffect(*HitActor, ImpactDamageHandle.EffectHandle);
		}
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

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	const TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> HitActors;

	UKismetSystemLibrary::SphereOverlapActors(this, InLocation, InDamage.ExplosionRadius,
		ObjectTypes, nullptr, ActorsToIgnore, HitActors);

	DrawDebugSphere(GetWorld(), InLocation, InDamage.ExplosionRadius,
		16, FColor::Red, false, 2.f, 0, 2.f);

	DrawDebugSphere(GetWorld(), InLocation, 2.f,
		16, FColor::Green, false, 2.f, 0, 2.f);
	
	for (const auto& Actor : HitActors)
	{
		if (!ensure(Actor)) continue;

		const bool bDied = ApplyDamageEffect(*Actor, InDamage.EffectHandle);

		if (InDamage.bApplyRocketJumpImpulse && !bDied && Actor == GetInstigator())
		{
			ApplyRocketJumpImpulse(Actor, InLocation, InDamage.ExplosionRadius);
		}
	}
}

bool AMETProjectile::ApplyDamageEffect(const AActor& InActor, const FGameplayEffectSpecHandle& InEffectHandle)
{
	if (ensure(InEffectHandle.IsValid()))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(&InActor))
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InEffectHandle.Data.Get());
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
	DamageTimerDelegate.BindUFunction(this, FName("ApplyDelayedDamage"), InOtherActor, InHitResult.ImpactPoint);
	GetWorldTimerManager().SetTimer(DamageTimerHandle, DamageTimerDelegate, DamageDelay, false);
}

void AMETProjectile::ApplyDelayedDamage(const AActor* InOtherActor, const FVector& InLocation)
{
	if (!ensure(IsValid(InOtherActor))) return;

	if (DelayedDamageHandle.bExplosive)
	{
		Explode(DelayedDamageHandle, InLocation);
	}
	else
	{
		ApplyDamageEffect(*InOtherActor, DelayedDamageHandle.EffectHandle);
	}

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
	}
}
