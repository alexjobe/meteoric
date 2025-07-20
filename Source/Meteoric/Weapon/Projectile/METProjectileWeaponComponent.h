// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METProjectileTypes.h"
#include "Components/ActorComponent.h"
#include "METProjectileWeaponComponent.generated.h"

USTRUCT()
struct FMETSpawnProjectileParams
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	AActor* Owner;

	UPROPERTY(Transient)
	APawn* Instigator;

	FMETProjectileDamageHandle ImpactDamageHandle;
	FMETProjectileDamageHandle DelayedDamageHandle;

	FMETSpawnProjectileParams()
		: Owner(nullptr)
		, Instigator(nullptr)
	{
		ImpactDamageHandle.DamageTiming = EDamageTiming::Impact;
		DelayedDamageHandle.DamageTiming = EDamageTiming::Delayed;
	}
};

USTRUCT()
struct FMETProjectileSpawnHandle
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	class AMETProjectile* Projectile;

	FTransform SpawnTransform;

	bool bTraceHit;
	
	FHitResult TraceHitResult;

	FMETProjectileSpawnHandle()
		: Projectile(nullptr)
		, bTraceHit(false)
	{}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETProjectileWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"), Category = "Projectile")
	TSubclassOf<class AMETProjectile> ProjectileClass;

	/* Number of projectiles fired per shot */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"), Category = "Projectile", meta = (ClampMin = 0))
	int32 NumProjectiles;

	/* If true, will perform a line trace a short distance along the projectile's projected path before spawning.
	 * If a hit is found, the impact will trigger immediately on spawn. 
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"), Category = "Projectile")
	bool bPerformSpawnTraceTest;

	/* How far ahead of projectile spawn (muzzle) to trace for impact */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"), Category = "Projectile", meta = (ClampMin = 0))
	float SpawnTraceDistance;

public:	
	UMETProjectileWeaponComponent();

	//~ Begin UActorComponent interface
	virtual void InitializeComponent() override;
	//~ End UActorComponent interface

	void FireProjectiles(const TArray<FMETSpawnProjectileParams>& InParams) const;

	/* Number of projectiles fired per shot */
	int32 GetNumProjectiles() const { return NumProjectiles; }

private:
	UPROPERTY(Transient)
	TObjectPtr<class UMETWeaponSpreadComponent> WeaponSpreadComponent;

	void SpawnProjectile(const FMETSpawnProjectileParams& InParams, FMETProjectileSpawnHandle& OutHandle) const;

	static void FireProjectile(const FMETProjectileSpawnHandle& InHandle);

	bool SpawnTraceTest(const FMETSpawnProjectileParams& InParams, const FTransform& InSpawnTransform, FHitResult& OutHitResult) const;
};
