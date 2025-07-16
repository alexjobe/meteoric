// Copyright Alex Jobe


#include "METProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"

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
	
	if (ImpactDamageHandle.IsValid())
	{
		UMETAbilitySystemUtils::AddHitResultToEffectSpec(ImpactDamageHandle.EffectHandle, SweepResult);

		if (ImpactDamageHandle.bExplosive)
		{
			Explode(ImpactDamageHandle);
		}
		else
		{
			ApplyDamageEffect(*OtherActor, ImpactDamageHandle.EffectHandle);
		}
	}

	if (DelayedDamageHandle.IsValid())
	{
		UMETAbilitySystemUtils::AddHitResultToEffectSpec(DelayedDamageHandle.EffectHandle, SweepResult);
		StartDelayedDamageTimer(OtherActor, OtherComp, SweepResult);
	}
	else
	{
		Destroy();
	}
}

void AMETProjectile::Explode(const FMETProjectileDamageHandle& InDamage) const
{
	const FVector ExplosionLocation = GetActorLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	const TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> HitActors;

	UKismetSystemLibrary::SphereOverlapActors(this, ExplosionLocation, InDamage.ExplosionRadius,
		ObjectTypes, nullptr, ActorsToIgnore, HitActors);

	DrawDebugSphere(GetWorld(), ExplosionLocation, InDamage.ExplosionRadius,
		16, FColor::Red, false, 2.f, 0, 2.f);
	
	for (const auto& Actor : HitActors)
	{
		if (ensure(Actor))
		{
			ApplyDamageEffect(*Actor, InDamage.EffectHandle);
		}
	}
}

void AMETProjectile::ApplyDamageEffect(const AActor& InActor, const FGameplayEffectSpecHandle& InEffectHandle)
{
	if (ensure(InEffectHandle.IsValid()))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(&InActor))
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InEffectHandle.Data.Get());
		}
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
	if (!ensure(IsValid(InOtherActor))) return;

	if (DelayedDamageHandle.bExplosive)
	{
		Explode(DelayedDamageHandle);
	}
	else
	{
		ApplyDamageEffect(*InOtherActor, DelayedDamageHandle.EffectHandle);
	}

	Destroy();
}
