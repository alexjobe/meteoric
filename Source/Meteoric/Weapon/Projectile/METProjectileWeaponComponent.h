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

public:	
	UMETProjectileWeaponComponent();

	//~ Begin UActorComponent interface
	virtual void InitializeComponent() override;
	//~ End UActorComponent interface

	void FireProjectiles(const TArray<FMETSpawnProjectileParams>& Params) const;

	/* Number of projectiles fired per shot */
	int32 GetNumProjectiles() const { return NumProjectiles; }

private:
	UPROPERTY(Transient)
	TObjectPtr<class UMETWeaponSpreadComponent> WeaponSpreadComponent;
	
	void SpawnProjectile(const FMETSpawnProjectileParams& Params) const;
};
