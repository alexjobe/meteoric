// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "METAmmoDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETAmmoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UMETAmmoDataAsset();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	FName Name;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag WeaponTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UGameplayEffect> EquippedEffectClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> ImpactDamageEffectClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DelayedDamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float ImpactDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DelayedDamage;
};
