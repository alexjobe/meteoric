// Copyright Alex Jobe


#include "METProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Meteoric/Meteoric.h"

AMETProjectile::AMETProjectile()
	: LifeSpan(10.f)
	, DamageDelay(.5f)
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

void AMETProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
}

void AMETProjectile::CollisionComponent_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !ensure(OtherActor)) return;
	if (bCollided) return;

	bCollided = true;
	
	if (ImpactDamageEffectHandle.IsValid())
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*ImpactDamageEffectHandle.Data.Get());
		}
	}

	if (DelayedDamageEffectHandle.IsValid())
	{
		StartDelayedDamageTimer(OtherActor, OtherComp, SweepResult);
	}
	else
	{
		Destroy();
	}
}

void AMETProjectile::StartDelayedDamageTimer(AActor* InOtherActor, UPrimitiveComponent* InOtherComponent, const FHitResult& InHitResult)
{
	ProjectileMovementComponent->StopMovementImmediately();
	SetActorLocation(InHitResult.ImpactPoint);
	AttachToComponent(InOtherComponent, FAttachmentTransformRules::KeepWorldTransform, InHitResult.BoneName);

	const FTimerDelegate DamageTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::ApplyDelayedDamage, InOtherActor);
	GetWorldTimerManager().SetTimer(DamageTimerHandle, DamageTimerDelegate, DamageDelay, false);
}

void AMETProjectile::ApplyDelayedDamage(AActor* InOtherActor)
{
	if (ensure(DelayedDamageEffectHandle.IsValid()) && ensure(IsValid(InOtherActor)))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InOtherActor))
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DelayedDamageEffectHandle.Data.Get());
		}
	}

	Destroy();
}
