// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "METWeaponLoadout.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETWeaponLoadout : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	TArray<TSubclassOf<AMETWeapon>> Weapons;
};
