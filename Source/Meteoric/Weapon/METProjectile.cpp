// Copyright Alex Jobe


#include "METProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMETProjectile::AMETProjectile()
	: LifeSpan(10.f)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComponent->InitSphereRadius(5.f);
	RootComponent = CollisionComponent;

	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::CollisionComponent_OnComponentBeginOverlap);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 20000.f;
	ProjectileMovementComponent->MaxSpeed = 20000.f;
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
	if (!DamageEffectHandle.IsValid()) return;

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DamageEffectHandle.Data.Get());
		}

		Destroy();
	}
}
