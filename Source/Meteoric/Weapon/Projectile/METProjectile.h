// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "METProjectileTypes.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "METProjectile.generated.h"

USTRUCT()
struct FMETDamageFXData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize ImpactPoint;

	UPROPERTY()
	FVector_NetQuantizeNormal ImpactNormal;

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	EMETDamageTiming DamageTiming;
};

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
	
	struct FMETAmmoDamageConfig* GetDamageConfig(const EMETDamageTiming& InDamageTiming) const;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float LifeSpan;

	/* How long to wait before delayed damage is applied. Only relevant if delayed damage has a valid effect handle */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float DamageDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion|RocketJump", meta = (ClampMin = 0))
	float RocketJumpForce;

	/* How much influence player movement has on the direction of rocket jump
	 * (0 = no bias, 1 = full movement direction) */
	UPROPERTY(EditDefaultsOnly, Category = "Explosion|RocketJump", meta = (ClampMin = 0, ClampMax = 1))
	float RocketJumpMovementInfluence;

	/* Z value added to the final impulse direction */
	UPROPERTY(EditDefaultsOnly, Category = "Explosion|RocketJump", meta = (ClampMin = 0, ClampMax = 1))
	float RocketJumpVerticalBias;

	/* If a shot contains multiple explosive projectiles (ex. shotgun), all nearby grouped projectiles contribute to a
	 * single explosion. This is the distance from the original impact that determines which projectiles are nearby */
	UPROPERTY(EditDefaultsOnly, Category = "Explosion|Cluster")
	float GroupedProjectileSearchRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|FX")
	TObjectPtr<USoundCue> InAirLoopSound;

	UPROPERTY(Transient, Replicated)
	TObjectPtr<class UMETAmmoDataAsset> AmmoType;

	/* Damage effect applied on impact */
	FGameplayEffectSpecHandle ImpactEffectHandle;

	/* Damage effect applied after a duration */
	FGameplayEffectSpecHandle DelayedEffectHandle;

	bool bOnlyCollideOnSweep;

	TArray<TWeakObjectPtr<AMETProjectile>> GroupedProjectiles;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void CollisionComponent_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Explode(const struct FMETAmmoDamageConfig& InDamageConfig, const FVector& InLocation, const EMETDamageTiming& InDamageTiming) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDamageFX(const FMETDamageFXData& InData);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayGroupedDamageFX(const TArray<FMETDamageFXData>& InDataArray) const;

	void PlayDamageFX(const FMETDamageFXData& InData) const;

private:
	void ExecuteDamage(const FHitResult& InHitResult, const AActor* HitActor, const EMETDamageTiming& InDamageTiming);

	/* Returns true if target dies */
	bool ApplyDamageEffect(const AActor& InActor, const EMETDamageTiming& InDamageTiming) const;
	
	void StartDelayedDamageTimer(AActor* InOtherActor, UPrimitiveComponent* InOtherComponent, const FHitResult& InHitResult);

	UFUNCTION()
	void ApplyDelayedDamage(const AActor* InActor, const FHitResult& InHitResult);

	void ApplyRocketJumpImpulse(const AActor* InActor, const FVector& InLocation, float InExplosionRadius) const;

	/* If a shot contains multiple explosive projectiles (ex. shotgun), all nearby grouped projectiles contribute to a
	 * single explosion. The final explosion radius is the sum of all contributing projectiles' explosion radii. */
	bool ComputeClusteredExplosionContribution(const FVector& InExplosionOrigin, const EMETDamageTiming& InDamageTiming, TArray<TStrongObjectPtr<AMETProjectile>>& OutContributingProjectiles, float& OutExplosionRadius) const;

	static bool ApplyClusteredDamage(const AActor& InActor, TArray<TStrongObjectPtr<AMETProjectile>>& InContributingProjectiles, const EMETDamageTiming& InDamageTiming);

	FTimerHandle DamageTimerHandle;

	bool bCollided;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};