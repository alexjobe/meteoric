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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag WeaponTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UGameplayEffect> EquippedEffectClass;
};
