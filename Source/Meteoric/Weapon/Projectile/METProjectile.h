// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "METProjectileTypes.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "METProjectile.generated.h"

UCLASS()
class METEORIC_API AMETProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> CollisionComponent;
	
public:	
	AMETProjectile();

	void Fire(const FVector& InDirection) const;
	void TriggerImpact(const FHitResult& InHitResult);

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float LifeSpan;

	/* How long to wait before delayed damage is applied. Only relevant if delayed damage has a valid effect handle */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float DamageDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|RocketJump", meta = (ClampMin = 0))
	float RocketJumpForce;

	/* How much influence player movement has on the direction of rocket jump
	 * (0 = no bias, 1 = full movement direction) */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|RocketJump", meta = (ClampMin = 0, ClampMax = 1))
	float RocketJumpMovementInfluence;

	/* Z value added to the final impulse direction */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|RocketJump", meta = (ClampMin = 0, ClampMax = 1))
	float RocketJumpVerticalBias;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|FX")
	TObjectPtr<class UParticleSystem> ImpactVfx;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|FX")
	TObjectPtr<class USoundCue> ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|FX")
	TObjectPtr<class USoundCue> InAirLoopSound;

	/* Damage applied on impact */
	FMETProjectileDamageHandle ImpactDamageHandle;

	/* Damage applied after a duration */
	FMETProjectileDamageHandle DelayedDamageHandle;

	bool bOnlyCollideOnSweep;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void CollisionComponent_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Explode(const FMETProjectileDamageHandle& InDamage, const FVector& InLocation) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayImpactFX(FVector_NetQuantize ImpactPoint, FVector_NetQuantizeNormal ImpactNormal, EPhysicalSurface SurfaceType);

private:
	FTimerHandle DamageTimerHandle;

	void Impact(const FMETProjectileDamageHandle& InDamageHandle, const FHitResult& InHitResult, const AActor* HitActor);

	// Returns true if target dies
	static bool ApplyDamageEffect(const AActor& InActor, const FGameplayEffectSpecHandle& InEffectHandle);
	
	void StartDelayedDamageTimer(AActor* InOtherActor, UPrimitiveComponent* InOtherComponent, const FHitResult& InHitResult);

	UFUNCTION()
	void ApplyDelayedDamage(const AActor* InActor, const FHitResult& InHitResult);

	void ApplyRocketJumpImpulse(const AActor* InActor, const FVector& InLocation, float InExplosionRadius) const;

	bool bCollided;
};