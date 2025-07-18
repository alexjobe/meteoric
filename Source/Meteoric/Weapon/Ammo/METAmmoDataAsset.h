// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "METAmmoDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FMETAmmoDamageConfig
{
	GENERATED_BODY()

	/* Gameplay Effect applied to target */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	/* Damage magnitude */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float Damage;

	/* If true, damage will be applied to all targets in a sphere */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Explosion")
	bool bExplosive;

	/* Radius of sphere explosion (if applicable) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Explosion")
	float ExplosionRadius;

	/* If true, explosion applies rocket jump impulse to instigator */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage|Explosion")
	bool bApplyRocketJumpImpulse;

	FMETAmmoDamageConfig()
		: DamageEffectClass(nullptr)
		, Damage(0.f)
		, bExplosive(false)
		, ExplosionRadius(0.f)
		, bApplyRocketJumpImpulse(false)
	{}
};

/**
 * 
 */
UCLASS()
class METEORIC_API UMETAmmoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/* Ammo name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	FName Name;

	/* Gameplay Tag of the associated weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag WeaponTag;

	/* Gameplay Effect applied to owner when ammo is equipped */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> EquippedEffectClass;

	/* Damage applied on impact */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FMETAmmoDamageConfig ImpactDamageConfig;

	/* Damage applied after a duration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FMETAmmoDamageConfig DelayedDamageConfig;
};
